# usart

## Overview

本稿では、ATtiny1616でUSARTを制御するための要点を整理します。対応するソースコードは `src/usart_hello` にあります。

## USARTペリフェラルの利用

### 初期化

データシート _24.3.1 初期化_ によれば、

> 1. TxD ピンの値を HIGH に設定し、必要に応じて XCK ピンを LOW に設定します(PORT.OUT 内のOUT[n])。
> 2. TxD ピン(および必要に応じて XCK ピン)を出力として設定します(PORT.DIR 内の DIR[n])。
> 3. baud レート(USART.BAUD)とフレーム フォーマットを設定します。
> 4. 動作モードを設定します(XCK ピン出力を同期モードで有効にします)。
> 5. 用途に応じてトランスミッタまたはレシーバを有効にします。
>
> 割り込み駆動による USART 動作のため、初期化中はグローバル割り込みを無効にする必要があります。

また _24.5.7 制御 B_ によれば、

> Bit 6 – TXEN: トランスミッタ イネーブル
> このビットに「1」を書き込む事で USART トランスミッタを有効にします。
> 有効にされたトランスミッタは、TxD ピンの通常のポート動作を上書きします。トランスミッタの無効化(TXEN への「0」の書き込み)は、実行中および保留中の送信が完了するまで(すなわち、送信シフトレジスタと送信バッファレジスタが送信データを格納している限り) 実行されません。
> 無効にされたトランスミッタは、TxD ポートを上書きしません。

さらに _24.5.10 baud レートレジスタ_ によれば、

> USART.BAUDL および USART.BAUDH レジスタペアにより、16 ビット値 USART.BAUD を形成します。

これに加え _表 24-2. baud レートレジスタ設定値の計算式_ を参照し……

USARTの基本的な初期化コードは以下のようになります。

```c
#define UART_TX_PIN 2  // PB2
#define BAUD_RATE 115200
#define USART_BAUD_RATE(BAUD_RATE) ((float)(F_CPU * 64 / (16 * (float)BAUD_RATE)) + 0.5)

void uart_init(void) {
    PORTB.DIRSET = (1 << UART_TX_PIN);
    USART0.BAUD = (uint16_t)USART_BAUD_RATE(BAUD_RATE);
    USART0.CTRLB = USART_TXEN_bm;
}
```

### 送信

データシート _24.3.2.3 データ送信 - USART トランスミッタ_ によれば、

> データ送信は、送信バッファ(USART.TXDATA 内の DATA)に送信するデータを書き込む事によって開始します。
> 送信バッファ内のデータは、シフトレジスタがエンプティになって次のフレームの送信準備が完了した時点でシフトレジスタへ転送されます。シフトレジスタは、アイドルステート中(実行中の送信なし)の場合に書き込まれるか、直前のフレームの最後のストップビットが送信された直後に書き込まれます。シフトレジスタにデータが書き込まれると、1 つの完全なフレームが送信されます。
> シフトレジスタ内のフレームが完全にシフトアウトされた時、送信バッファに新しいデータが格納されていなければ、送信完了割り込みフラグ(USART.STATUS 内の TXCIF) がセットされ、有効にされていれば割り込みが生成されます。
> TXDATA は、データレジスタ エンプティフラグ(USART.STATUS 内の DREIF) がセットされている時にのみ書き込み可能です。このフラグは、レジスタがエンプティであり新しいデータの書き込みが可能である事を示します。

すなわち、単一バイトを送信する関数は以下のようになります。

```c
void uart_send(uint8_t data) {
    while(!(USART0.STATUS & USART_DREIF_bm));
    USART0.TXDATAL = data;
}
```

ここまでの内容をまとめたコードは `usart_hello/main.c` に示しています。

### 送信バッファ割込みを活用する

`USART0.STATUS` を `while` でずっと監視し続けるのは骨が折れるので、送信バッファ割込みを活用してみます。

データシート _24.3.4 割り込み_ によれば、

> 0x02 DRE データレジスタエンプティ割り込み 送信バッファがエンプティ/新しいデータの受信準備完了
>
> 割り込み条件が発生するとステータス レジスタ(USART.STATUS) 内の対応する割り込みフラグがセットされます。制御 A レジスタ(USART.CTRLA) 内の対応するビットに書き込む事により、割り込み要因を有効または無効にします。
> 有効にされた割り込み要因の割り込みフラグがセットされた時、割り込み要求が生成されます。割り込み要求は、割り込みフラグがクリアされるまでアクティブです。
> 割り込みフラグをクリアするための詳細な方法は、USART.STATUS レジスタの説明を参照してください。

_24.5.6 制御 A_ によれば、

> Bit 5 – DREIE: データレジスタ エンプティ割り込みイネーブル
>
> このビットは、データレジスタ エンプティ割り込み(割り込みベクタ DRE)を有効にします。有効にされた割り込みは、USART.STATUS レジスタの DREIF がセットされた時にトリガされます。

ということなので、データポインタと送信完了フラグを用意し……

```c
static const char* tx_data = NULL;
static volatile bool tx_empty = true;
```

こんな感じでISRを構成し……

```c
// USART0データエンプティ割込み
ISR(USART0_DRE_vect) {
    if (*tx_data != '\0') {
        USART0.TXDATAL = *tx_data++;
    } else {
        tx_empty = true;
        USART0.CTRLA &= ~USART_DREIE_bm;
    }
}
```

関数 `uart_send` はこんな感じにすれば……

```c
void uart_send(const char* const data) {
    while (!tx_empty);

    tx_data = data;
    tx_empty = false;
    USART0.CTRLA |= USART_DREIE_bm;
}
```

送信中にCPUをブロックすることがなくなりました 🎉
ここまでの内容をまとめたコードは `usart_send_interrupt/main.c` に示しています。

### 受信

続いてデータ受信を試してみます。

データシート _24.5.7 制御 B_ によれば、

> Bit 7 – RXEN: レシーバ イネーブル
>
> このビットに「1」を書き込む事で USART レシーバを有効にします。有効にされたレシーバは、RxD ピンの通常のポート動作を上書きします。
> レシーバを無効にすると受信バッファがフラッシュされ FERR、BUFOVF、PERR フラグは無効になります。GENAUTO および LINAUTO モードの場合、レシーバを無効にすると baud レート自動検出ロジックはリセットされます。

また _24.3.2.4 データ受信 - USART レシーバ_ によれば、

> 24.3.2.4 データ受信 - USART レシーバ
>
> レシーバを有効にすると、RxD ピンはレシーバのシリアル入力として機能します。ピン n の方向は、ポートの方向レジスタで入力(PORT_DIR.DIR[n] = 0)として設定されている必要があります(これは既定値のピン設定です)。
>
> フレームの受信
>
> レシーバは、有効なスタートビットを検出した時にデータの受信を開始します。スタートビットに続く各ビットは、baud レートまたは XCK クロックに同期してサンプリングされ、フレーム内の最初のストップビットを受信するまで、受信シフトレジスタへシフトインされます。レシーバは 2 つ目のストップビットを無視します。
> 最初のストップビットを受信して完全なシリアルフレームが受信シフトレジスタに格納された時点で、シフトレジスタの内容が受信バッファへ転送されます。すると USART.STATUS内の受信完了割り込みフラグ(RXCIF) がセットされ、有効にされている場合は割り込みが生成されます。
> 受信バッファの内容は、RXDATA (USART.RXDATAL 内の DATA[7:0]と USART.RXDATAH 内のDATA[8]で構成) を読み出す事によって読み出せます。RXDATA は、USART.STATUS 内の受信完了割り込みフラグ(RXCIF)がセットされている時に読み出す必要があります。
> 8 ビットに満たないフレームを使う場合、未使用の最上位ビットは「0」として読み出されます。9 ビット キャラクタを使う場合、第 9 ビット(USART.RXDATAH 内の DATA[8])を下位バイト(USART.RXDATAL 内の DATA[7.0]) より先に読み出す必要があります。

すなわち、単一バイトを受信する関数はこのように書くことができます。

```c
uint8_t uart_recv() {
    while (!(USART0.STATUS & USART_RXCIF_bm));
    return USART0.RXDATAL;
}
```

ここまでの内容をまとめたコードは `usart_recv/main.c` に示しています。

### 受信バッファ割込みを活用する

受信バッファにも割込みがあるので、これを活用しましょう。

データシート _24.5.6 制御 A_ によれば、

> Bit 7 – RXCIE: 受信完了割り込みイネーブル
>
>このビットは、受信完了割り込み(割り込みベクタ RXC)を有効にします。有効にされた割り込みは、USART.STATUS レジスタの RXCIF がセットされた時にトリガされます。

ということなので、データポインタを用意し……

```c
static volatile uint8_t data = 0x00;
static volatile bool rx_available = false;
```

こんな感じでISRを構成し……

```c
// USART0受信完了割込み
ISR(USART0_RXC_vect) {
    data = USART0.RXDATAL;
    rx_available = true;
}
```

関数 `uart_init` 内で受信バッファ割込みを有効化し……

```c
void uart_init(void) {
    cli();

    USART0.BAUD = (uint16_t)USART_BAUD_RATE(BAUD_RATE);
    USART0.CTRLB = USART_RXEN_bm;
    USART0.CTRLA |= USART_RXCIE_bm;
}
```

関数 `uart_recv` はこんな感じにすれば……

```c
uint8_t uart_recv() {
    while (!rx_available);
    rx_available = false;
    return data;
}
```

受信中にCPUをブロックすることがなくなりました 🎉
ここまでの内容をまとめたコードは `usart_recv_interrupt/main.c` に示しています。

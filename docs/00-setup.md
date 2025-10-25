# はじめに

ATtiny1616は、シリーズ **tinyAVR 1** に属するAVRファミリのひとつです。
それまでのATtiny(13, 85, 2313, …………)とは内部構造レベルで大きく異なり、さらに洗練されたアーキテクチャが採用されています。

本稿では、このMCUを扱うのに最低限必要な情報を整理します。

## Spec

搭載ペリフェラル等については割愛します。

- 周波数: 20MHz
- フラッシュメモリ: 16kB
- RAM: 2kB
- EEPROM: 512B

## Delivery

2025年現在、主要なディストリビュータから安定して購入できます。パッケージはSSOPやQFNなどが展開されています。

価格帯は概ね140~160円程度と、スペックの割には安価な部類といえます (少なくとも、Arduinoに載っていたATmega系統よりは……はるかに……)

- [mouser](https://www.mouser.jp/c/?q=ATtiny1616)
- [Digi-Key](https://www.digikey.jp/ja/products/filter/%E3%83%9E%E3%82%A4%E3%82%AF%E3%83%AD%E3%82%B3%E3%83%B3%E3%83%88%E3%83%AD%E3%83%BC%E3%83%A9/685?s=N4IgTCBcDaIIIBUAuBLAdgTwIwDZcgF0BfIA)
- 秋月電子通商
  - [SSOP](https://akizukidenshi.com/catalog/g/g130947/)
  - [DIP化キット](https://akizukidenshi.com/catalog/g/g130229/)

## Programming

公式のIDEのほか、開発には `avr-gcc`, 書込みには `avrdude` がそれぞれ使えます。
書込みには _Serial UPDI_ と呼ばれるI/Fを利用できます。USBシリアル変換器とダイオード、抵抗からなる非常に単純な回路です。

NOTE: `avr-gcc` をHomebrewでインストールする際はバージョンに注意してください(9系はtiny1 AVRに対応していないようです)。

## References

- [tinyAVR® 1シリーズ入門 - Microchip](https://ww1.microchip.com/downloads/en/DeviceDoc/30010176C.pdf)
- [tinyAVR® Microcontrollers - Microchip](https://ww1.microchip.com/downloads/en/DeviceDoc/30010176C.pdf)
- [AVRDUDE: 5.3 SerialUPDI Programmer](https://avrdudes.github.io/avrdude/7.3/avrdude_21.html)

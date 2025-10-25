# ATtiny1616で遊ぶ

## Overview

tinyAVR1シリーズのMCU **ATtiny1616** を買ったので、適当に遊んでみる

## Usage

1. configure

   ```sh
   cmake --preset target
   ```
  
2. build

   ```sh
   cmake --build --preset target
   ```

3. flash

   ```sh
   avrdude -p attiny1616 -c serialupdi -P /path/to/port -U flash:w:./build/target/src/<dir_name>/<target_name>
   ```

## Requirements

このリポジトリはサンプルプログラムの集合体であり、これ自体がCMakeプロジェクトとして構成されています。
実行には以下のソフトウェアおよび依存関係が必要です。

1. CMake >= 3.23: [preset](https://cmake.org/cmake/help/latest/manual/cmake-presets.7.html)機能を使うため、3.23以上が必要です。
2. [Enchan1207/cmake-avr](https://github.com/Enchan1207/cmake-avr)

## License

This repository is published under [MIT License](LICENSE).

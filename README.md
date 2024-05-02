# TINY202_IR_REMOTE_ISR for ATMEL STUDIO 7.0
![TINY202_IR_REMOTE 2024-05-02 233458](https://github.com/todopapa/TINY202_IR_REMOTE_ISR/assets/16860878/7a59901e-49d1-468d-9323-dc31d36176b7)

## はじめに
### TB-B-GONEをベースに汎用リモコンを作る
前回 TV-B-GONE V1.2のファームウェアをATMEL STUDIO7.0に移植しました。
今回は、下記の内容を実装します。
1.CPUをATTINY85から、ATTINY202に移植する。
2.TVの電源ON/OFF制御だけでなく、汎用のIRリモコンとして使用できるようにする。

### このプロジェクトの概要
TV-B-GONEはライセンスフリーで、Adafruit社が最初の[TV-BーGONEキット](https://www.adafruit.com/product/73)を出したのは2005年？前後で、8PINのAVRマイコン ATTINY85 CPUを使って制御しています。  
ハードウェアとファームウェアは当初のV1.0から、いまはV1.2に進化しています。  

![TV-B-GONE_ORG](https://github.com/todopapa/TV_B_GONE_1R1TV_B_GONE_1R1_AV_AV/assets/16860878/55073afe-6188-482c-b98b-c53ae8d22c4e)TV_B_GONE_1R1TV_B_GONE_1R1_AV_AV
Adafruit TV-BーGONE V1.1　回路図

## **TV_B_GONE_11_AVR**

は、AVRマイコン ATTNY85 のファームウェアです。  

* ATTINY85 8pin DIP 対応  
* ATMEL STUDIO（現Microchip Studio）対応  
* 書き込みは AVR ISP MK２または、パラレルプログラマーTL866IIを使用する  
* [Arduino as ISP](https://www.instructables.com/How-to-Program-an-Attiny85-From-an-Arduino-Uno/)  
   Arduino Uno もArduino as ISPとして、ATTINY85の書き込み機としてつかえる  

## 回路図

![ATTINY202 IR REMOTE ](https://github.com/todopapa/TINY202_IR_REMOTE_ISR/assets/16860878/8d0a1272-e935-4369-9447-d28aac719b77)
トドお父さん版 TINY202_IR_REMOTE V1.1 for ATMEL STUDIO　回路図  

回路はAdafruitのV1.1をベースにしているので単純です。  
ATTINY85のRESETピンにタクトSWを入れ、タクトSWを押したときにTINY85にハードRESETが入り、プログラムが先頭から走ります。  
写真はタクトSWが２つになっていますが、将来的に空きピンにタクトSWを入れれば、ON/OFF以外の他のコントロールもできます。  
(その場合入力ピンに内部プルアップ指定をします）  
IR LEDは、PB0, PB1の２つを使ってますが、片方だけでもOKです。  
動作確認用の赤LEDは3.3kΩを入れてます。最近のLEDは感度が高いのでこれでOKのようです。(暗い場合は1kΩ等に変更してください）  
IR LEDには直列に1Ωの抵抗をいれてますが、実験的には無くてもOKの様です。（Q1トランジスタのベース電流ｘHfeで流れる）  

## ATMEL STUDIOプロジェクトファイル

頭のTV_B_GONE_11_AVR.atsln がプロジェクトファイルです。TV_B_GONE_11_AVRフォルダ内には  
* main.c : メインプログラム  
* NAcodes.c : 北米/アジア仕向け 各社テレビのON/OFF 赤外線コードの構造体  
* util.c : デバッグ用ソフトUart (未使用）  
* Debugフォルダ：生成したHEXファイル、ELFファイルが入る  

## 使い方

タクトSWを押すとテレビのリモコンコードの発信が始まります。  
各社リモコンコード発信の動作中は赤LEDが点滅して動作中を知らせます。  
全部のコードが発振されるまで（テレビが消えるまで）押し続けてください。  
全部発信されると、最後に4回LEDが短く点灯した後、消灯します。  
動作が終わると、TINY85はSleepモードに入り、動作電流は < 1uA になります。  
Adafruitによると、このガジェットに対応するテレビは、下記のようです。家電店等でテストするのは止めたほうがいいかもしれせん。  
Acer, Admiral, Aiko, Alleron, Anam National, AOC, Apex, Baur, Bell&Howell, Brillian, Bush, Candle, Citizen, Contec, Cony, Crown, Curtis Mathes, Daiwoo, Dimensia, Electrograph, Electrohome, Emerson, Fisher, Fujitsu, Funai, Gateway, GE, Goldstar, Grundig, Grunpy, Hisense, Hitachi, Infinity, JBL, JC Penney, JVC, LG, Logik, Loewe, LXI, Majestic, Magnavox, Marantz, Maxent, Memorex, Mitsubishi, MGA, Montgomery Ward, Motorola, MTC, NEC, Neckermann, NetTV, Nikko, NTC, Otto Versand, Palladium, Panasonic, Philco, Philips, Pioneer, Portland, Proscan, Proton, Pulsar, Pye, Quasar, Quelle, Radio Shack, Realistic, RCA, Samsung, Sampo, Sansui, Sanyo, Scott, Sears, SEI, Sharp, Signature, Simpson, Sinudyne, Sonolor, Sony, Soundesign, Sylviana, Tatung, Teknika, Thompson, Toshiba, Universum, Viewsonic, Wards, White Westinghouse, Zenith

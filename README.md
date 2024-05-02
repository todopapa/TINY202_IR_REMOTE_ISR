# TINY202_IR_REMOTE_ISR for ATMEL STUDIO 7.0
<img src="https://github.com/todopapa/TINY202_IR_REMOTE_ISR/assets/16860878/7a59901e-49d1-468d-9323-dc31d36176b7" width="480">
<!-- ![TINY202_IR_REMOTE 2024-05-02 233458](https://github.com/todopapa/TINY202_IR_REMOTE_ISR/assets/16860878/7a59901e-49d1-468d-9323-dc31d36176b7)
-->
## はじめに
### TB-B-GONEをベースに汎用リモコンを作る　　
前回 TV-B-GONE V1.2のファームウェアをATMEL STUDIO7.0に移植しました。  
今回は、下記の内容を実装します。  

1.CPUをATTINY85から、ATTINY202に移植する。  
2.TVの電源ON/OFF制御だけでなく、汎用のIRリモコンとして使用できるようにする。  

### このプロジェクトの概要
TV-B-GONEはライセンスフリーで、Adafruit社が最初の[TV-BーGONEキット](https://www.adafruit.com/product/73)を出したのは2005年？前後で、8PINのAVRマイコン ATTINY85 CPUを使って制御しています。  
ハードウェアとファームウェアは当初のV1.0から、いまはV1.2に進化しています。  

![TV-B-GONE Adafruit schematics_2024-05-02 235112](https://github.com/todopapa/TINY202_IR_REMOTE_ISR/assets/16860878/751ad074-005d-4b4e-a7a8-8ae2165690f8)TV_B_GONE_1R1TV_B_GONE_1R1_AV_AV  
Adafruit TV-BーGONE V1.1　回路図  

## **TINY202_IR_REMOTE_ISR1**
は、AVRマイコン ATTNY202 のファームウェアです。 　 
* ATTINY202 8pin DIP 対応  　
* ATMEL STUDIO（現Microchip Studio）対応  　
* 書き込みは UPDIを使用する　　  　　
* [Adafruit UPDI Friend]([https://www.instructables.com/How-to-Program-an-Attiny85-From-an-Arduino-Uno/](https://learn.adafruit.com/adafruit-updi-friend/overview))  　　
   Arduino Unoもファームを書き換え jtag2UPDIとして、ATTINY202の書き込み機としてつかえるようです。　　　  

## 回路図
![ATTINY202 IR REMOTE ](https://github.com/todopapa/TINY202_IR_REMOTE_ISR/assets/16860878/8d0a1272-e935-4369-9447-d28aac719b77)   
トドお父さん版 TINY202_IR_REMOTE V1.1 for ATMEL STUDIO　回路図  

回路はAdafruitのV1.1をベースにして、ATTINY85からATTINY202に変更しています。  

また、オリジナルはRESETを"L"にしてプログラムを先頭から走らせるようにしていました。  
ATTINY202はRESETがUPDIピンにアサインされてハードRESETが使えません。  
このため、ピンチェンジ割り込みを使ってプログラムを制御します。  

今回はPA1=SW1，PA6=SW2、PA7=SW0として、タクトSWを3つ実装して３種のIRコードを発信できるようにしています。  
(この場合、各入力ピンに内部プルアップ指定、割り込み指定をします）  

出力のIR LEDは、PA3を、動作確認用のLEDはPA2をアサインしています。  
動作確認用のLEDは2kΩを入れてます。最近のLEDは感度が高いのでこれでOKのようです。(暗い場合は1kΩ等に変更してください）  

IR LEDはPA3のTIMER1とCMP0の比較が一致した時に、WO0がトグルする構成になっています。  
PA3のWO0信号は330Ωを介しQ1トランジスタ2SC1815のベースに入力しています。  
Q1のコレクタに２つのIR LEDを並列に接続してパルス的に大電流で駆動します。  

IR LEDには直列に1Ωの抵抗をいれてますが、実験的には無くてもOKの様です。  
（Q1トランジスタのベース電流ｘhfeで流れる）    

## ATMEL STUDIOプロジェクトファイル　　
頭のTINY202_IR_REMOTE_ISR1.atsln がプロジェクトファイルです。TV_B_GONE_11_AVRフォルダ内には  
* main.c : メインプログラム  
* IRcodes.c : 北米/アジア仕向け 各社テレビのON/OFF 赤外線コードの構造体  
* util.c : デバッグ用ソフトUart (未使用）  
* Debugフォルダ：生成したHEXファイル、ELFファイルが入る  

## 使い方
今回はPanasonicの天井灯のリモコンを制御します。  
・タクトSWを押すとリモコンコードの発信が始まります。  

・リモコンは"mode切り替え", "UP", "DOWN" の3種類のコードを発信します。  

・リモコンコード発信の動作中は赤LEDが点滅して動作中を知らせます。  

・全部のコードが発振されるまで（テレビが消えるまで）押し続けてください。    

・ボタンを離してIRコードの発信が終わると、最後に4回LEDが短く点灯した後、動作が終わります。  

・動作が終わると、TINY202はSleepモードに入り、動作電流は < 1uA になります。 (約0.1uA)  

・このリモコンの動作モードは、ボタンを押している間連続で同じIRコードを発信します。  

・単発でよい場合は、コードの226行目、252行目をコメントアウトしてください。  
　　226　// while ((~PORTA.IN & SW0_PIN) | (~PORTA.IN & SW1_PIN) | (~PORTA.IN & SW2_PIN)){ //while SW0 or SW1 or SW2 is pressed then continue IR LED flashing  
　　252 //	}  

## リモコンコード
<img src="https://github.com/todopapa/TINY202_IR_REMOTE_ISR/assets/16860878/f1b21625-a868-4491-9a7b-8a4c05b9795c" width="360">
パナソニック天井灯リモコン　HK9337　
このリモコンはM5060という三菱のリモコン用ICを使用しています。
もうこのICは手に入りませんが、コンパチのPT2560という中華ICのデータシートが見つかりましたので、これを参考に
必要なコードを解析していきます。（実際はオシロスコープによる波形解析を行った）




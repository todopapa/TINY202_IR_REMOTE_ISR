# TINY202_IR_REMOTE_ISR for ATMEL STUDIO 7.0
<img src="https://github.com/todopapa/TINY202_IR_REMOTE_ISR/assets/16860878/7a59901e-49d1-468d-9323-dc31d36176b7" width="480">
<!-- ![TINY202_IR_REMOTE 2024-05-02 233458](https://github.com/todopapa/TINY202_IR_REMOTE_ISR/assets/16860878/7a59901e-49d1-468d-9323-dc31d36176b7)　-->

## はじめに
### TB-B-GONEをベースに汎用リモコンを作る　　
前回 TV-B-GONE V1.2のファームウェアをATMEL STUDIO7.0に移植しました。  
今回は、下記の内容を実装します。  

1.CPUをATTINY85から、ATTINY202に移植する。  
2.TVの電源ON/OFF制御だけでなく、汎用のIRリモコンとして使用できるようにする。  
3.そのためにスイッチを３個つけて、ピンチェンジ割り込みを使って３種類の制御ができるようにする。  

### このプロジェクトの概要
TV-B-GONEはライセンスフリーで、Adafruit社が最初の[TV-BーGONEキット](https://www.adafruit.com/product/73)を出したのは2005年？前後で、8PINのAVRマイコン ATTINY85 CPUを使って制御しています。  
ハードウェアとファームウェアは当初のV1.0から、いまはV1.2に進化しています。  
<img src="https://github.com/todopapa/TINY202_IR_REMOTE_ISR/assets/16860878/751ad074-005d-4b4e-a7a8-8ae2165690f8)" width="480">
<!-- ![TV-B-GONE Adafruit schematics_2024-05-02 235112](https://github.com/todopapa/TINY202_IR_REMOTE_ISR/assets/16860878/751ad074-005d-4b4e-a7a8-8ae2165690f8)TV_B_GONE_1R1TV_B_GONE_1R1_AV_AV  -->
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
動作確認用のLEDは2kΩを入れてます。最近のLEDは感度が高いのでこれでOKのようです。  
(暗い場合は1kΩ等に変更してください）  

IR LEDはPA3のTIMER1とCMP0の比較が一致した時に、WO0がトグルする構成になっています。  
PA3のWO0信号は330Ωを介しQ1トランジスタ2SC1815のベースに入力しています。  
Q1のコレクタに２つのIR LEDを並列に接続してパルス的に大電流で駆動します。  

IR LEDには直列に1Ωの抵抗をいれてますが、実験的には無くてもOKの様です。  
（Q1トランジスタのベース電流ｘhfeで流れる）    

## ATMEL STUDIOプロジェクトファイル　　
頭のTINY202_IR_REMOTE_ISR1.atsln がプロジェクトファイルです。TINY202_IR_REMOTE_ISR1フォルダ内には  
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
もうこのICは手に入りませんが、コンパチのPT2560[PT2560](url)という中華ICのデータシートが見つかりましたので、これを参考に    
必要なコードを解析していきます。（実際はオシロスコープによる波形解析を行った）  

<!<!----![PT2560 CODE 2024-05-03 011738](https://github.com/todopapa/TINY202_IR_REMOTE_ISR/assets/16860878/00e489bc-4ffc-4ae1-a2fb-4cbaf296b434)  -->
<img src="https://github.com/todopapa/TINY202_IR_REMOTE_ISR/assets/16860878/00e489bc-4ffc-4ae1-a2fb-4cbaf296b434" width="640">   

この図でLeader Codeは3.57ｍSと1.80ｍS、”0”は420/480uS、"1"は420/1320uSでした。  
コードは下記でした。　キャリア周波数は38KHzがICの仕様ですが、440kHzのセラミック発振器を使って36.37kHzでした。  
これに従って、IRcodes.c　を修正しています。  

mode切り替え：custom code 34h、4Ah、90h　/　data code   14h、84h END  
UP　　　　　：custom code 34h、4Ah、90h　/　data code 　54h、C4h END  
DOWN　　　　：custom code 34h、4Ah、90h　/　data code 　D4h、44h END  

## スリープ時電流
ATTINY202のスリープ時電流を測定しました。  
0.1uAで、ATTINY85の0.2～0.3uAより明らかに小さいです。  
<!--![IMG_0471](https://github.com/todopapa/TINY202_IR_REMOTE_ISR/assets/16860878/f6b65397-b703-45a6-a5ef-4505950a0ba0)-->
<img src="https://github.com/todopapa/TINY202_IR_REMOTE_ISR/assets/16860878/f6b65397-b703-45a6-a5ef-4505950a0ba0" width="480">  

## クロックの設定 (8MHz)
今回は、ATTINY85のクロックに合わせてクロックは8MHz設定にします。  
（assmblerのNOPでタイミングを調整した delay_ten_us() という10uS単位のディレイ関数があるためです）

ATTINY202のクロック系統の構成は下記のように、MAIN_CLOCK源の設定とプリスケーラになっています。  
この出力CLK_PERがTIMER/COUNTER等に入力されます。    
<img src="https://github.com/todopapa/TINY202_IR_REMOTE_ISR/assets/16860878/0daeca7c-4309-414f-91b6-8611f89cafcf" width="480">  

起動RESET後のクロック系統の動作は、下記データシートに書かれているように、Main Clock 20/16MHz　Prescaler 6分周になっています。  
これを今回は　Main clock 16MHz Prescaler 2分周にして、クロックを8MHzに設定しします。
<img src="https://github.com/todopapa/TINY202_IR_REMOTE_ISR/assets/16860878/a853f67f-75dc-4714-aa42-43a20f36e04d" width="480">  
<!--![TINY202 default clock 2024-05-03 10
2832](https://github.com/todopapa/TINY202_IR_REMOTE_ISR/assets/16860878/a853f67f-75dc-4714-aa42-43a20f36e04d)-->









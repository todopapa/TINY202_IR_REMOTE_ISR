# TINY402/202_IR_REMOTE_ISR for ATMEL STUDIO 7.0
This is a new AVR ATTINY 402/202series IR Remote Controller w/multiple keys input using pin change ISR  
based on TV-B-GONE program  
<img src="https://github.com/todopapa/TINY202_IR_REMOTE_ISR/assets/16860878/7a59901e-49d1-468d-9323-dc31d36176b7" width="480">
<!-- ![TINY202_IR_REMOTE 2024-05-02 233458](https://github.com/todopapa/TINY202_IR_REMOTE_ISR/assets/16860878/7a59901e-49d1-468d-9323-dc31d36176b7)　-->

## はじめに
### TV-B-GONEをベースに汎用リモコンを作る　　
前回 TV-B-GONE V1.2のファームウェアをATMEL STUDIO7.0に移植しました。  
今回は、下記の内容を実装します。  

1.CPUをATTINY85から、ATTINY402/202に移植する。  
2.TVの電源ON/OFF制御だけでなく、汎用のIRリモコンとして使用できるようにする。  
3.そのためにスイッチを３個つけて、ピンチェンジ割り込みを使って３種類の制御ができるようにする。  
　New ATTINYのタイマー/ISRを使った事例がすくないので、情報を調査してその内容をまとめるのも、今回の目的の一つです。  

### このプロジェクトの概要
TV-B-GONEはライセンスフリーで、Adafruit社が最初の[TV-BーGONEキット](https://www.adafruit.com/product/73)を出したのは2005年？前後で、  
8PINのAVRマイコン ATTINY85 CPUを使って制御しています。  
ハードウェアとファームウェアは当初のV1.0から、いまはV1.2に進化しています。  
<img src="https://github.com/todopapa/TINY202_IR_REMOTE_ISR/assets/16860878/751ad074-005d-4b4e-a7a8-8ae2165690f8)" width="480">
<!-- ![TV-B-GONE Adafruit schematics_2024-05-02 235112](https://github.com/todopapa/TINY202_IR_REMOTE_ISR/assets/16860878/751ad074-005d-4b4e-a7a8-8ae2165690f8)TV_B_GONE_1R1TV_B_GONE_1R1_AV_AV  -->
**Adafruit TV-BーGONE V1.1　回路図**  

## **TINY202_IR_REMOTE_ISR1**
は、AVRマイコン ATTNY202 のファームウェアです。 　 
* ATTINY202 8pin DIP 対応  　
* ATMEL STUDIO（現Microchip Studio）対応  　
* 書き込みは UPDIを使用する　　  　　
* [Adafruit UPDI Friend]([https://www.instructables.com/How-to-Program-an-Attiny85-From-an-Arduino-Uno/](https://learn.adafruit.com/adafruit-updi-friend/overview))  　　
   Arduino Unoもファームを書き換え jtag2UPDIとして、ATTINY202の書き込み機としてつかえるようです。　　　  

## 回路図
<img src="https://github.com/todopapa/TINY202_IR_REMOTE_ISR/assets/16860878/d0b8481c-817d-47d6-a535-f8ba9e405f51" width="360">   

**トドお父さん版 TINY202_IR_REMOTE V1.1 for ATMEL STUDIO　回路図**

回路はAdafruitのV1.1をベースにして、ATTINY85からATTINY202に変更しています。  
（もちろんATTINY402でも対応できます）  

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

IR LEDには直列に10Ωの抵抗をいれてますが、実験的には無くてもOKの様です。  
（Q1トランジスタのベース電流ｘhfeで流せるが実際はIR LEDの特性でリミットがかかる）    

## ATMEL STUDIOプロジェクトファイル　　
頭のTINY202_IR_REMOTE_ISR1.atsln がプロジェクトファイルです。TINY202_IR_REMOTE_ISR1フォルダ内には  
* main.c : メインプログラム  
* IRcodes.c : 元は北米/アジア仕向け 各社テレビのON/OFF 赤外線コードの構造体  
  　　　　　　　→ 自分が使う装置の赤外線コードに書き換える,TINY202は容量が少ないのが　5～10くらいが入る  
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

**パナソニック天井灯リモコン　HK9337**   
このリモコンはM5060という三菱のリモコン用ICを使用しています。   
もうこのICは手に入りませんが、コンパチの[PT2560](https://pdf.dzsc.com/60-/PT2560-002.pdf)という中華ICのデータシートが見つかりましたので、これを参考に    
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
<img src="https://github.com/todopapa/TINY202_IR_REMOTE_ISR/assets/16860878/f6b65397-b703-45a6-a5ef-4505950a0ba0" width="320">  

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

プログラムのこの部分です。データシートにあるようにプロテクトされている部分ですから_PROTECTED_WRITE() で当該レジスタに書き込みます。
```
void SYSCLK_init(void) {
	// SYSCLK 8MHz (16MHz /2)
	/* Set the Main clock to internal 16MHz oscillator*/
	_PROTECTED_WRITE(CLKCTRL.MCLKCTRLA, CLKCTRL_CLKSEL_OSC20M_gc);
	/* Set the Main clock divider is / 2 and Main clock prescaler enabled. */
	_PROTECTED_WRITE(CLKCTRL.MCLKCTRLB, CLKCTRL_PDIV_2X_gc | CLKCTRL_PEN_bm); //0x01
}
```
これだけでは、内部クロックに20MHzが出力されるので、(FUSE.OSCCFG) fuse で16MHzの設定をする必要があります。

<img src="https://github.com/todopapa/TINY202_IR_REMOTE_ISR/assets/16860878/2330607a-ac0a-46a9-95c3-d84d2e2e9961" width="640"> 

FUSE.OSCCFGレジスタの設定は下記、デフォールトは0x02（20MHz）、これを0x01（16MHz）に変更します。  
<img src="https://github.com/todopapa/TINY202_IR_REMOTE_ISR/assets/16860878/407393ac-5878-47cb-8575-22901651c689" width="640"> 

FUSEの設定は、プログラムに構造体で記載してHEXファイルにFUSEデータを入れて、書き込み器でFLashデータと一緒にFUSEデータを  
書き込む方法もあるようなのですが、これはなぜかうまくいきませんでした。　  
困りはててAVR Freakで調べて、AVRDudeで書き込む方法があると知ったので、もっぱらこちらでFUSEを書き込んでいます。  
fuse2がFUSE.OSCCFGレジスタになります。  

Fuses on ATtiny1614 with AVRDUDE  
https://www.avrfreaks.net/s/topic/a5C3l000000BqV9EAK/t391706  
WDTCFG = fuse0, BODCFG = fuse1, OSCCFG = fuse2.....  
ディレクトリ　D:\Program Files (x86)\AVRDUDESS　にて  (fuse2の指定だけでOKです。COM5は自分の環境に合わせて下さい）  
avrdude -Cavrdude.conf -c serialupdi -p t202 -P COM5 -U fuse0:w:0x00:m -U fuse1:w:0x00:m -U fuse2:w:0x1:m     
  
## AVRDUDESSを使ったプログラムとFUSE書き込みの方法  
UPDIでのHV対応プログラマも作りましたので、GUIのAVRDUDESSを使った書き込み方法について、こちらに詳しく書いています。
https://github.com/todopapa/UPDI_HV_WRITER-w-RESET  
  
**後日追記**：ChatGPT先生にお伺いを立てたのですが、やはりプログラムでFUSEを書き換える方法はないようです。  
最近は、AVRDUDEをGUI化した、AVRDUDESSをよく使っています。  
こちらに、AVRDUDESSを使ってプログラムとFUSEを書く方法を説明しているので、参考にしてください。 
**ATTINY202/402のFUSEビットをAVRDUDESSを使って書く方法**
https://ameblo.jp/powpher/entry-12863906067.html   
  
## TIMER/COUNTERの使い方  
ここがATTINY85と大きく変わっているので、やっぱり説明せんばなりませんね。
MicroChipの技術資料 [TB3217 Getting Started with Timer/Counter Type A (TCA)](https://www.microchip.com/en-us/application-notes/tb3217)) を参考にしました。
TINY85ではTCNT0とTCNT1という２つの8bitカウンタ/タイマがあって、OCRnX, OCRnBに設定した値と比較して任意の周波数やディレイを作ってました。
これに対しTINY202ではTCA0とTCB0という2つの16bitカウンタ/タイマがあって前段のPrescaler、CNTレジスタ、PERレジスタ、CMPnレジスタで制御しています。

```c
void xmitCodeElement(uint16_t ontime, uint16_t offtime ) {
	// start TCA0 outputting the carrier frequency to IR emitters on CMP0 WO0 (PA3, pin 7)
	/* set waveform output on PORT A */
	TCA0.SINGLE.CTRLB = TCA_SINGLE_CMP0EN_bm // enable compare channel 0
	| TCA_SINGLE_WGMODE_FRQ_gc;	// set Frequency mode
	/* disable event counting */
	TCA0.SINGLE.EVCTRL &= ~(TCA_SINGLE_CNTEI_bm);
	/* set frequency in FRQ mode */
	// TCA0.SINGLE.CMP0 = PERIOD_EXAMPLE_VALUE;   //the period time (FRQ) already set in main()
	/* set clock source (sys_clk/div_value) */
	TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV1_gc   // set clock source (sys_clk/1)
	| TCA_SINGLE_ENABLE_bm; /* and start timer */
	// keep transmitting carrier for onTime
	delay_ten_us(ontime);
	//for debug continue emitting test
	// while(1);
	
	// turn off output to IR emitters on 0C1A (PB1, pin 6) for offTime
	TCA0.SINGLE.CTRLA &= ~(TCA_SINGLE_ENABLE_bm); /* stop timer to set bit "0" */
	TCA0.SINGLE.CTRLB = 0;	// CTRLB register RESET add for forced OUTPUT "L"
	PORTA.OUTCLR = IRLED_PIN;  // turn off IR LED

	delay_ten_us(offtime);
}
```
1.CTRLBの設定：TCA0をFRQモード (WOnに出力を出す)、コンペアレジスタにCMP0を選択（WO0に出力される）  
2.EVCTRLの設定：EVENT入力を止める    
3.CMP0レジスタに比較値を入れ出力周波数を決める（これはmain()で設定するので、ここはコメントアウト）    
4.CTRLAの設定：プリスケーラの設定1/1、TCA0をEnable（起動）する    
止めるときは、CTRLAでTCA0をDisableするだけでなく、CTRLBも0x00を入れてPA3へのWO0の出力を止めないと  
PORTA.OUTCLR = IRLED_PIN(PIN3_bm);　だけでは”L"にならず、IR LEDが光りっぱなしになるので注意してください。  

なお、コンペアレジスタ CMPnを選択すると出力はWOnになります。　下記の出力ピンリストで参照ください。
PORTMUXで代替ピンの使用も可能になります。WO0のデフォールトの出力はPA3ですが、PA7にも出力できます。
<img src="https://github.com/todopapa/TINY202_IR_REMOTE_ISR/assets/16860878/922acc81-6f77-4c1d-9545-99a526dc1c91" width="640">

## Pin Change InterruptとSleepの使い方
  コードを参考に説明しますね。    
  下の参考資料 ”pin change interruptも参考になると思います。  

  main.c　の記述  
  ```c
int main(void) {
	SYSCLK_init();
	PIN_init();
	while(1){	
		// pin change interrupt for buttons
		PORTA.PIN1CTRL |= PORT_ISC_LEVEL_gc;  // SW1  set ISR 
		PORTA.PIN6CTRL |= PORT_ISC_LEVEL_gc;  // SW2  set ISR
		PORTA.PIN7CTRL |= PORT_ISC_LEVEL_gc;  // SW0  set ISR
		sei();          // Interrupt enable
		
		// Sleep mode setting and enable
		set_sleep_mode(SLEEP_MODE_PWR_DOWN);
		sleep_enable();	// Pin change interrupt setting

		sleep_cpu();    // put CPU into Power Down Sleep Mode

		// after CPU wake
		PORTA.PIN1CTRL &= ~(PORT_ISC_LEVEL_gc);  // Turn off pin sense interrupt for SW1
		PORTA.PIN6CTRL &= ~(PORT_ISC_LEVEL_gc);  // Turn off pin sense interrupt for SW2
		PORTA.PIN7CTRL &= ~(PORT_ISC_LEVEL_gc);  // Turn off pin sense interrupt for SW3
  ```
1. PORTA.PINnCTRLで各ピンの割り込みを設定する
　下記 PINnCTRLレジスタの記述のISC[2:0] で割り込みを設定します。  
　・PORT_ISC_LEVEL_gcは0x05で/* Sense low Level */ローレベルで割り込みの設定です。(FallingでもOK)    
 　 なお、プルアップの有無もこのレジスタで各ピンごとに設定します。  
     
2. set_sleep_modeでsleep時の動作を設定する。  
　・SLEEP_MODE_PWR_DOWNではSleep時に割り込みを残して最大パワーダウンする設定です。  、
   
3. sleep_enable()でsleepイネーブルにする、
 sleepのctrlAレジスタにSleep_modeとsleep_enableの設定があります。Sleepイネーブルにします。  
   
4. sleep_cpu()でSleepに入る。
   電力を低減するために割り込み以外の動作を止めて、各スイッチからのピン変化割込み待ちになります。  
　これは、前の2.3.を含め、定義をみると下記のようになっています。  
  
 ```c
 SLEEP.CTRLA = SLEEP_MODE_PWR_DOWN | SLEEP_SEN_bm; // Power Downモード + Sleep Enable
__asm__ __volatile__ ("sleep");  // ここでCPUがスリープ状態に入る
  ```
asmのsleep命令とはなんでしょうか？ これはコンパイラが内部命令に置き換えるコマンドのようです。
ChatGPT先生によるとAVR Instruction Set Manual (AVR命令セットマニュアル) 内にあり、  
機械語のSLEEPの二モニック 0x9588に変換されるとのことです。  
  
5. 割り込みが入るとWakeupして、ISR (PORTA_PORT_vect)の処理に入ります
   ATTINY85と違い、new ATTINYでは各ピンごとに割り込み要因としてフラグが立ちます。
   割り込みが入ったピンを確認し、ピンごとの処理を行うことができます。
　 今回は、それぞれのピンをポーリングして押し下げたスイッチに対応した値を変数Playにいれます。
　割り込みフラグも全て一緒にクリアします。
　ちなみにATTINY85では、ISR処理に入ると自動で全て割り込みフラグはクリアされます。

6. main()に戻って、処理を始める。
   while(1) 文の// after CPU wake以降の処理を行う。まず、各ピン変化割り込みをディスエーブルします。    
　 押されたスイッチに対応する処理（IRコマンド発光）を行います。  

割り込み処理  ISR(PORTA_PORT_vect)の記述  

  ```c
// Pin change interrupt
ISR (PORTA_PORT_vect) {
	// pin change detection for each buttons and decide Play #
	int Buttons = PORTA.IN;
	if ((Buttons & SW0_PIN) == 0) Play = 0;
	else if ((Buttons & SW1_PIN) == 0) Play = 1;
	else if ((Buttons & SW2_PIN) == 0) Play = 2;
	else Play = 0;

	PORTA.INTFLAGS =  PORT_INT1_bm | PORT_INT6_bm | PORT_INT7_bm ;        // Clear PA1,PA6,PA7 interrupt flag
}
  ```  


PINnCTRLレジスタの記述  

<img src="https://github.com/user-attachments/assets/3ce80260-3c24-464d-80a1-a7f80b016dda" width="640">  

  
INT Flagレジスタの記述  

<img src="https://github.com/user-attachments/assets/141c6219-518a-4229-bac3-775b58bb8870" width="640">  


## 動作確認のYoutube画像  
https://youtu.be/LKW64Bx1fvs  

## 他のATTINY402/202開発参考資料

pin change Interrupt  
https://www.avrfreaks.net/s/topic/a5C3l000000UaC5EAK/t152923  

Technoblogy New ATtiny Low Power  
http://www.technoblogy.com/show?2RA3  

New ATTINY コード記述方法　Direct Port Manipulation  
https://github.com/SpenceKonde/megaTinyCore/blob/master/megaavr/extras/Ref_DirectPortManipulation.md  

Microchip  Getting Started with GPIO TB3229  
https://www.microchip.com/en-us/application-notes/tb3229  

Getting started ATtiny 1 & 0 series / Application Notes  
https://www.avrfreaks.net/s/topic/a5C3l000000UZTjEAO/t150173  

ATTINY202/402/802 datasheet  
https://ww1.microchip.com/downloads/aemDocuments/documents/MCU08/ProductDocuments/DataSheets/ATtiny202-204-402-404-406-DataSheet-DS40002318A.pdf  

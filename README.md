# asulE_L298
asulE Motor library

아두이노 l298n 라이브러리

사용방법
=======


1. 라이브러리 포함
-----------------
<pre>
<code>
#include "AsulL298.h"
</code>
</pre>


2. 라이브러리 변수 생성
---------------------
<pre>
<code>
AsulL298 l298(6,7,5,8,9,10);
        IN1, IN2, ENA, IN3, IN4, ENB
</code>        
</pre>


3. 라이브러리 시작
-----------------
<pre>
<code>
l298.attach();
이때 핀 설정을 함.
</code>        
</pre>


4. 모션 함수들
---------------------
<pre>
<code>
l298.goForward();  //앞으로
l298.goBackward(); //뒤로
l298.turnLeft();   //왼쪽 돌기
l298.turnRight();  //오른쪽 돌기
l298.moveMotor(joyX, joyY, speed); //조이스틱으로 제어하기
joyX : 0 ~ 1023
joyY : 0 ~ 1023
speed : 현재 사용하지 않음.
</code>        
</pre>


moveMotor 함수는 아래 링크를 참고함.

http://www.impulseadventure.com/elec/robot-differential-steering.html


 

# javax_websocket_demo

javax.websocket를 이용한 채팅서버와 클라이언트 데모

##채팅 내 명령어
채널 변경
/set room 1(숫자 1~10)
* 기본 접속 채널 0 (대기방 개념. 채팅이 불가능하며 다시 0번으로 들어올 수는 없음)

닉네임 변경
/set nickname ㅇㅇㅇ(변경할 닉네임)
* 닉네임 입력 텍스트 박스의 내용으로 초기 닉네임 설정

##구성
Tomcat 7 + Maven

##작업환경
Windows7, Eclipse (Spring Tool Suite 3.7.1)

## javax, tomcat
java에서 사용가능 했던 websocket 리스트
###1. javax.websocket
###2. org.apache.tomcat.websocket

* org.apache.catalina.websocket 패키지는 Deprecated 되었음
>https://tomcat.apache.org/tomcat-7.0-doc/web-socket-howto.html

#### 최종적으로 javax.websocket 선정
범용성(glassfish,tomcat..) & 예제가 더 보기 좋아서...

##작업 내역
(2015-10-07)

 1. echo서버 올림 (copy & paste)
2. 클라이언트 연결 부분 수정 (소캣 1회 open 후 여러번의 메시지 전송 가능하게)

(2015-10-08)

1. 메시지 수신 시 접속되어 있는 다른 클라이언트들에게 broadcast 하도록 변경
2. ui 변경 조금.. 

(2015-10-12)

1. 채널(room)내 사용자들간의 대화 (채널 변경 시에도 소캣은 연결)
2. 닉네임 변경 가능

(2015-10-16)

1. 채널 인원 제한 설정
2. 구동 시 메소드 실행
3. 클래스 구조 변경
##echo 서버 구성에 대한 참고 문서
>https://blog.idrsolutions.com/2013/12/websockets-an-introduction/

##채팅 메시지 전송 방법에 대한 참고 문서
>http://hmkcode.com/first-time-java-api-for-websocket/

## 현재 프로젝트의 문제점
* 미지원 브라우저에 대한 정보 표기 안함



## 기타
####WebSocket
 
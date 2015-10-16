<html>
<head>
<meta http-equiv="content-type" content="text/html; charset=UTF-8">
</head>
<body>
	<meta charset="utf-8">
	<title>Chat</title>
	<script language="javascript" type="text/javascript">
        	
			//서버 주소를 변경해야 정상작동
            var wsUri = "ws://10.10.1.22:8080/websocket/echo";
            
            function init() {
            	bt_open_socket=document.getElementById("open_socket");
            	bt_close_socket=document.getElementById("close_socket");
            	
                output = document.getElementById("output");

            	bt_close_socket.disabled = true; 
            }

            //접속버튼 누를 경우
            function open_socket(){
            	websocket = new WebSocket(wsUri);
                
            }
            function close_socket(){
                websocket.close()
                
                //재 접속을 위한 버튼 상태 변경
                bt_close_socket.disabled = true; 
                bt_open_socket.disabled = false; 
            }
            
            function send_message() {
            	if(textID.value!="")
	                doSend(textID.value);
            }
            function onOpen(evt) {
                writeToScreen("Connected to Endpoint!");
            }
            function onMessage(evt) {
                writeToScreen(evt.data);
            }
            function onError(evt) {
                writeToScreen('ERROR: ' + evt.data);
            }
            function doSend(message) {
                websocket.send(message);
                document.getElementById("textID").value="";
            }
            function writeToScreen(message) {
                var pre = document.createElement("p");
                pre.style.wordWrap = "break-word";
                pre.innerHTML = message;
                 
                output.appendChild(pre);
                output.scrollTop = output.scrollHeight;   
            }
            
            function change_nickname() {  
            	websocket.send("");
            }
            
            function waitForSocketConnection(socket, callback){
                setTimeout(
                    function(){
                        if (socket.readyState === 1) {
                            if(callback !== undefined){
                                callback();
                            }
                            return;
                        } else {
                            waitForSocketConnection(socket,callback);
                        }
                    }, 5);
            };
            
            window.addEventListener("load", init, false);
            
            window.onbeforeunload = function (evt) {
            	alert("A");
            	close_socket();
            };
        </script>
	<h1 style="text-align: center;">WebSocket Chat Client</h1>
	<br>
	<div style="text-align: center;">

		<div>
			<input id="nickname" name="nickname" placeholder="Input your Nickname" type="text"
				onKeyPress="if (event.keyCode==13){change_nickname();event.returnValue=false}">	
			<br>
		</div>
		<input id="open_socket" onclick="open_socket()" value="Open WebSocket"
			type="button"> <input id="close_socket"
			onclick="close_socket()" value="Close WebSocket" type="button">
		<div>
			<input onclick="send_message()" value="Send" type="button"> <input
				id="textID" name="message" value="" type="text" placeholder=""
				onKeyPress="if (event.keyCode==13){ send_message();event.returnValue=false}"><br>
		</div>
	</div>
	<div id="output" style="max-height: 80%; overflow-y: scroll;"></div>
</body>
</html>

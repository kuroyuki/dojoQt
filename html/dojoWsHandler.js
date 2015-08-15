
function connectWebSocket(Uri)
{
	websocket = new WebSocket(Uri);
	websocket.onopen = function(evt) { onOpen(evt) };
	websocket.onclose = function(evt) { onClose(evt) };
	websocket.onmessage = function(evt) { onMessage(evt) };
	websocket.onerror = function(evt) { onError(evt) };
}


function onOpen(evt)
{
	console.log("CONNECTED");
}

function onClose(evt)
{
	console.log("DISCONNECTED");
}

function onMessage(evt)
{
	var json = JSON.parse(evt.data);

	if(json.command == "an"){
		//"{"axon":{"x":1,"y":3,"z":5},"command":"an","id":3,"pos":{"x":1,"y":3,"z":0}}"
		var pos = json.pos;
		var axon = json.axon;
		var id = json.id;
		var size = 0.5;

		sessionStorage.id = [pos,axon,size];
		addNode([pos.x, pos.y, pos.z], [axon.x,axon.y,axon.z], size);
	    
	}
	else if(json.command == "as"){
		//"{"command":"as","length":1,"permability":1,"source":0,"target":2}"
		var sourceData = sessionStorage[json.source];
		var targetData = sessionStorage[json.target];
	}
	else if(json.command == "ai"){
	    

	}
	else if(json.command == "ao"){
	    
	}

	console.log(JSON.stringify(json));                
}

function onError(evt)
{
	console.log(">ERROR: " + evt.data);
}

function doSend(message)
{
	console.log("SENT: " + message); 
	websocket.send(message);
}
		
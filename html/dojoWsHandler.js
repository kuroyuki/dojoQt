
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
	console.log(JSON.stringify(json));      

	if(json.command == "an"){
		//"{"axon":{"x":1,"y":3,"z":5},"command":"an","id":3,"pos":{"x":1,"y":3,"z":0}}"
		var pos = json.pos;
		var axon = json.axon;
		var id = json.id;
		var size = json.size;

		var nodeData = {
			"axon" : axon,
			"pos" : pos,
			"size" : size
		};

		sessionStorage.setItem(id, JSON.stringify(nodeData));

		addNode(nodeData.pos, nodeData.axon, nodeData.size);
	}
	else if(json.command == "as"){
		//"{"command":"as","length":1,"permability":1,"source":0,"target":2}"
		
		var source = JSON.parse(sessionStorage.getItem(json.source));
		var target = JSON.parse(sessionStorage.getItem(json.target));

		if(source === null){
			source = new Object();
			source.axon = {"x":0,"y":0,"z":-10};
		}
		if(target === null){
			target = new Object();
			target.pos = {"x":0,"y":0,"z":10};
		}

		bindNodes(source.axon, target.pos);
	}
	else if(json.command == "ai"){
	    

	}
	else if(json.command == "ao"){
	    
	}
     
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
		
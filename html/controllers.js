'use strict';

app.controller("MainController", function(Websocket, Tables, SceneDrawing){
  //connect to dojo Server
	Websocket.Ws.$on('$open', function () {
     console.log('WS opened');
  });

  Websocket.Ws.$on('an', function (data) {
    console.log('add neuron '+JSON.stringify(data));
	data.sources = [];
	data.targets = [];


    Tables.addNeuron(data);
	SceneDrawing.add(data.pos, data.axon, data.size);

  });

  Websocket.Ws.$on('as', function (data) {
    console.log('add synapse'+JSON.stringify(data));
	//
	var sourceNeuron = Tables.getNeuron(data.source);
	var targetNeuron = Tables.getNeuron(data.target);

	//if both neurons exist
	if(sourceNeuron != null && targetNeuron != null){
		//update source with target
		sourceNeuron.targets.push(targetNeuron.id);
		Table.updateNeuron(sourceNeuron);

		var source = {
			id : data.source,
			length : data.length,
			perm : data.perm
		}
		targetNeuron.sources.push(source);
		Table.updateNeuron(targetNeuron);

		//draw it
		SceneDrawing.bind(sourceNeuron.axon, targetNeuron.pos);
	}
  });

  Websocket.Ws.$on('un', function (data) {
		console.log('update neuron '+JSON.stringify(data));		
		Tables.updateNeuron(data.id, data);
  });
  Websocket.Ws.$on('us', function (data) {
    console.log('update synapse '+JSON.stringify(data));
  });
	Websocket.Ws.$on('spike', function (data) {
    console.log('spike from '+JSON.stringify(data));
  });
});

app.controller("WidgetController", function(Websocket, Tables){
    this.neurons = Tables.neurons;

    this.newPos = {};
    this.newAxon = {};
	this.newSize = 1;

	this.newSynapse = {};

	this.selectedNeuron = {};
	this.selectedSynapse = {};

	this.tab = 1;
	this.selectTab = function(setTab){
		this.tab = setTab;
	}
	this.isSelected = function (checkTab) {
		return this.tab === checkTab;
	}
	this.selectNeuron = function(neuron){
		this.selectedNeuron = {};
		this.selectedNeuron = neuron;
	}
	this.selectSource = function(source){
		this.selectedSynapse.source = source;
		this.selectedSynapse.target = this.selectedNeuron.id;
	}
	this.selectTarget = function(target){
			this.selectedSynapse.source = this.selectedNeuron.id;
			this.selectedSynapse.target = target;
	}
	this.addNeuron = function(){
      var data = {
        pos : {
					x : parseFloat(this.newPos.x),
					y : parseFloat(this.newPos.y),
					z : parseFloat(this.newPos.z)
				},
        axon : {
					x : parseFloat(this.newAxon.x),
					y : parseFloat(this.newAxon.y),
					z : parseFloat(this.newAxon.z)
				},

				size : this.newSize
      };

			//send new neuron request to WS server
      Websocket.Ws.$emit('an', data);

      console.log('request new neuron at '+JSON.stringify(data));

			//clear input fields
      this.newPos = {};
      this.newAxon = {};
      this.newSize = 1;
    };
		this.addSynapse = function(){
				var data = {
					source : parseInt(this.newSynapse.source),
					target : parseInt(this.newSynapse.target)
				}
				//send new neuron request to WS server
	      Websocket.Ws.$emit('as', data);

	      console.log('request new synapse between '+JSON.stringify(data));

				//clear input fields
	      this.newSynapse = {};
		}
		this.addToWatch = function(){
			var data = {
				id : parseInt(this.selectedNeuron.id)
			}
			//send new neuron request to WS server
			Websocket.Ws.$emit('sfs', data);

			console.log('add syanpse to watch '+JSON.stringify(data));
		}
});
app.controller("ChartController", function(ChartDrawing){
	ChartDrawing.Data[0] = [10,11,12,13,13,14,20,0];
	ChartDrawing.Data[1] = [0,1,2,3,3,4,25,10];
});

'use strict';

/* Services */


// Demonstrate how to register services
// In this case it is a simple value service.

app.factory('Tables', function () {
  var table = [];
  return {
    neurons : table,
    getNeuron : function(id){
      var data;

      for(var i=0; i<table.length; i++)
        if(table[i].id == id)
          data = table[i];

      return data;
    },
    addNeuron : function (data){      
       table.push(data);
    },
    updateNeuron : function (data){
       for(var i=0; i<table.length; i++)
        if(table[i].id == data.id){
          table[i] = data;
          return;
        }
    } 
  }
});
app.factory('Websocket', function ($websocket){
	//connect to dojo Server
	var ws = $websocket.$new('ws://localhost:10167'); // instance of ngWebsocket, handled by $websocket service

	return {Ws : ws};

});
app.service('ChartDrawing', function(){
  var data = {
  	  labels: ["January", "February", "March", "April", "May", "June", "July"],
  	  datasets: [
  	      {
  	          label: "My First dataset",
  	          fillColor: "rgba(220,220,220,0.2)",
  	          strokeColor: "rgba(220,220,220,1)",
  	          pointColor: "rgba(220,220,220,1)",
  	          pointStrokeColor: "#fff",
  	          pointHighlightFill: "#fff",
  	          pointHighlightStroke: "rgba(220,220,220,1)",
  	          data: []
  	      },
  	      {
  	          label: "My Second dataset",
  	          fillColor: "rgba(151,187,205,0.2)",
  	          strokeColor: "rgba(151,187,205,1)",
  	          pointColor: "rgba(151,187,205,1)",
  	          pointStrokeColor: "#fff",
  	          pointHighlightFill: "#fff",
  	          pointHighlightStroke: "rgba(151,187,205,1)",
  	          data: []
  	      }
  	  ]
  	};
  var options = {
    ///Boolean - Whether grid lines are shown across the chart
    scaleShowGridLines : true,
    //String - Colour of the grid lines
    scaleGridLineColor : "rgba(0,0,0,.05)",
    //Number - Width of the grid lines
    scaleGridLineWidth : 1,
    //Boolean - Whether to show horizontal lines (except X axis)
    scaleShowHorizontalLines: true,
    //Boolean - Whether to show vertical lines (except Y axis)
    scaleShowVerticalLines: true,
    //Boolean - Whether the line is curved between points
    bezierCurve : true,
    //Number - Tension of the bezier curve between points
    bezierCurveTension : 0.4,
    //Boolean - Whether to show a dot for each point
    pointDot : true,
    //Number - Radius of each point dot in pixels
    pointDotRadius : 4,
    //Number - Pixel width of point dot stroke
    pointDotStrokeWidth : 1,
    //Number - amount extra to add to the radius to cater for hit detection outside the drawn point
    pointHitDetectionRadius : 20,
    //Boolean - Whether to show a stroke for datasets
    datasetStroke : true,
    //Number - Pixel width of dataset stroke
    datasetStrokeWidth : 2,
    //Boolean - Whether to fill the dataset with a colour
    datasetFill : true,
    //String - A legend template
    legendTemplate : "<ul class=\"<%=name.toLowerCase()%>-legend\"><% for (var i=0; i<datasets.length; i++){%><li><span style=\"background-color:<%=datasets[i].strokeColor%>\"></span><%if(datasets[i].label){%><%=datasets[i].label%><%}%></li><%}%></ul>"
	};
	var ctx = document.getElementById("myChart").getContext("2d");
	var myLineChart = new Chart(ctx).Line(data, options);

  return {Labels : data.labels, Data : [data.datasets[0].data, data.datasets[1].data]};
});
app.service("SceneDrawing", function($window){
  var scene, renderer, camera, person, controls;

  // set some camera attributes
	var VIEW_ANGLE = 45,
	    ASPECT = $window.innerWidth / $window.innerHeight,
	    NEAR = 0.1,
	    FAR = 20000;

	// get the DOM element to attach to
	// - assume we've got jQuery to hand
	var container = document.getElementById('container');

	// create a WebangularGL renderer, camera
	// and a scene
	renderer = new THREE.WebGLRenderer();

	// start the renderer
	renderer.setSize($window.innerWidth , $window.innerHeight);

	// attach the render-supplied DOM element
	container.appendChild( renderer.domElement );


	camera = new THREE.PerspectiveCamera(  VIEW_ANGLE,
	                                ASPECT,
	                                NEAR,
	                                FAR  );
	// the camera starts at 0,0,0 so pull it back
	camera.position.y = 7;
	camera.position.z = 20;

	// controls = new THREE.OrbitControls( camera );
	// controls.damping = 0.2;
	// controls.addEventListener( 'change', render );

	scene = new THREE.Scene();

	// create a point light
	var pointLight = new THREE.PointLight( 0xFFFFFF );

	// set its position
	pointLight.position.x = 10;
	pointLight.position.y = 50;
	pointLight.position.z = 130;

	// add to the scene
	scene.add(pointLight);

	// create a point light
	var upperLight = new THREE.PointLight( 0xFFFFFF );

	// set its position
	upperLight.position.x = -50;
	upperLight.position.y = 200;
	upperLight.position.z = -50;

	// add to the scene
	scene.add(upperLight);

	//Grid
	var geometry = new THREE.Geometry();
	geometry.vertices.push( new THREE.Vector3( - 50, 0, 0 )  );
	geometry.vertices.push( new THREE.Vector3( 50, 0, 0 ) );

	var linesMaterial = new THREE.MeshLambertMaterial(
	{
	    color: 0x444444
	});

	for ( var i = 0; i <= 20; i ++ ) {

		var line = new THREE.Line( geometry, linesMaterial );
		line.position.z = ( i * 5 ) - 50;
		scene.add( line );

		var line = new THREE.Line( geometry, linesMaterial );
		line.position.x = ( i * 5 ) - 50;
		line.rotation.y = 90 * Math.PI / 180;
		scene.add( line );

	}

	window.addEventListener( 'resize', onWindowResize, false );

	animate();

  function render(){
  	renderer.render(scene, camera);
  }
  function onWindowResize() {

  	camera.aspect = $window.innerWidth / $window.innerHeight;
  	camera.updateProjectionMatrix();

  	renderer.setSize( $window.innerWidth, $window.innerHeight );

  	render();
  }

  function animate() {

  	requestAnimationFrame(animate);
  	//controls.update();

  }
  var addNode = function (pos, axon, size){
  	// create the sphere's material
  	var sphereMaterial = new THREE.MeshLambertMaterial(
  	{
  	    color: 0x99EE99
  	});

  	var geometryPos = new THREE.SphereGeometry( size, size, size );

  	var meshPos = new THREE.Mesh( geometryPos, sphereMaterial );
  	meshPos.position.x = pos.x;
  	meshPos.position.y = pos.y;
  	meshPos.position.z = pos.z;

  	scene.add( meshPos );

  	var geometryAxon = new THREE.SphereGeometry( size/3, size/3, size/3 );

  	var meshAxon = new THREE.Mesh( geometryAxon, sphereMaterial );
  	meshAxon.position.x = axon.x;
  	meshAxon.position.y = axon.y;
  	meshAxon.position.z = axon.z;

  	scene.add( meshAxon );

  	var lineGeometry = new THREE.Geometry();
  	lineGeometry.vertices.push(new THREE.Vector3(pos.x, pos.y, pos.z));
  	lineGeometry.vertices.push(new THREE.Vector3(axon.x, axon.y, axon.z));

  	var lineAxon = new THREE.Line(lineGeometry, sphereMaterial);

  	scene.add( lineAxon );

  	render();
  }
  var bindNodes = function(source, target){
  	var lineGeometry = new THREE.Geometry();
  	lineGeometry.vertices.push(new THREE.Vector3(source.x, source.y, source.z));
  	lineGeometry.vertices.push(new THREE.Vector3(target.x, target.y, target.z));
  	var lineMaterial = new THREE.MeshLambertMaterial(
  	{
  	    color: 0xCC9999
  	});
  	var line3 = new THREE.Line(lineGeometry, lineMaterial);
  	scene.add(line3);

  	render();
  }
  return {add : addNode, bind : bindNodes};
})

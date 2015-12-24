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
    var chartData = [];

    var chart = new CanvasJS.Chart("myChart",
            {
                zoomEnabled: true,
                title :{
                    text: "Neurons spike activity"
                },
                data: chartData
            });
    var updateInterval = 20;

    var updateChart = function () {
        for(var i=0;i<chartData.length; i++){
            //set 0 to all charts
            chartData[i].dataPoints.push({x:Date.now(), y: 0});

            while(chartData[i].dataPoints[0].x < Date.now()-5000){
                chartData[i].dataPoints.shift();
            }
        }
        //redraw chart
        chart.render();

    };

    // update chart after specified time.
    setInterval(function(){updateChart()}, updateInterval);

    function addChart(id) {
        var newChart = {
            type: "area",
            label : id,
            xValueType: "dateTime",
            dataPoints: []
        }

        chartData.push(newChart);

        console.log('add neuron to chart '+id);
    }
    function updateData(id, value){
        for(var i=0;i<chartData.length;i++){
            if(chartData[i].label === id){
                chartData[i].dataPoints.shift();

                chartData[i].dataPoints.push({x: Date.now(), y: value});

                break;
            }
        }
    }
    function removeChart(id){

    }

    return {
        add : addChart,
        spike : updateData ,
        remove : removeChart
    }
});
app.service("SceneDrawing", function($window){
    var camera, controls, scene, renderer, light;

    init();
    animate();

    function init() {

        scene = new THREE.Scene();

        renderer = new THREE.WebGLRenderer();
        renderer.setPixelRatio( window.devicePixelRatio );
        renderer.setSize( window.innerWidth, window.innerHeight );

        var container = document.getElementById( 'container' );
        container.appendChild( renderer.domElement );

        camera = new THREE.PerspectiveCamera( 60, window.innerWidth / window.innerHeight, 1, 1000 );
        camera.position.y = 7;
        camera.position.z = 20;

        controls = new THREE.OrbitControls( camera, renderer.domElement );

        controls.enableDamping = true;
        controls.dampingFactor = 0.25;
        controls.enableZoom = true;

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
        // lights

        light = new THREE.DirectionalLight( 0xffffff );
        light.position.set( 1, 1, 1 );
        scene.add( light );

        light = new THREE.DirectionalLight( 0x002288 );
        light.position.set( -1, -1, -1 );
        scene.add( light );

        light = new THREE.AmbientLight( 0x222222 );
        scene.add( light );

        window.addEventListener( 'resize', onWindowResize, false );

    }

    function onWindowResize() {

        camera.aspect = window.innerWidth / window.innerHeight;
        camera.updateProjectionMatrix();

        renderer.setSize( window.innerWidth, window.innerHeight );

    }

    function animate() {

        requestAnimationFrame( animate );

        controls.update(); // required if controls.enableDamping = true, or if controls.autoRotate = true

        render();

    }

    function render() {

        renderer.render( scene, camera );

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

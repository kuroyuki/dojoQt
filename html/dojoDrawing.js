var scaleView = 1;
var scene;
var renderer;
var camera;

function setupDrawScene(){
	// set the scene size
	var WIDTH = 800,
	    HEIGHT = 600;

	// set some camera attributes
	var VIEW_ANGLE = 45,
	    ASPECT = WIDTH / HEIGHT,
	    NEAR = 1,
	    FAR = 1000;

	// get the DOM element to attach to
	// - assume we've got jQuery to hand
	var $mainScene = $('#mainScene');

	// create a WebGL renderer, camera
	// and a scene
	renderer = new THREE.WebGLRenderer();
	
	// start the renderer
	renderer.setSize(WIDTH, HEIGHT);

	// attach the render-supplied DOM element
	$mainScene.append(renderer.domElement);

	camera = new THREE.PerspectiveCamera(  VIEW_ANGLE,
	                                ASPECT,
	                                NEAR,
	                                FAR  );
	// the camera starts at 0,0,0 so pull it back
	camera.position.z = 20;

	scene = new THREE.Scene();
	// and the camera
	scene.add(camera);

	// create a point light
	var pointLight = new THREE.PointLight( 0xFFFFFF );

	// set its position
	pointLight.position.x = 10;
	pointLight.position.y = 50;
	pointLight.position.z = 130;

	// add to the scene
	scene.add(pointLight);

	// draw!

	renderer.render(scene, camera);

	}

function addNode([X,Y,Z], [AX,AY,AZ], size){
	// create the sphere's material
	var sphereMaterial = new THREE.MeshLambertMaterial(
	{
	    color: 0xCC0000
	});

	var geometryPos = new THREE.SphereGeometry( scaleView*size, scaleView*size, scaleView*size );

	var meshPos = new THREE.Mesh( geometryPos, sphereMaterial );
	meshPos.position.x = scaleView*X;
	meshPos.position.y = scaleView*Y;
	meshPos.position.z = scaleView*Z;

	scene.add( meshPos );

	var geometryAxon = new THREE.SphereGeometry( scaleView*size/2, scaleView*size/2, scaleView*size/2 );

	var meshAxon = new THREE.Mesh( geometryAxon, sphereMaterial );
	meshAxon.position.x = scaleView*AX;
	meshAxon.position.y = scaleView*AY;
	meshAxon.position.z = scaleView*AZ;

	scene.add( meshAxon );

	var lineGeometry = new THREE.Geometry();
	lineGeometry.vertices.push(new THREE.Vector3(scaleView*X,scaleView*Y,scaleView*Z));
	lineGeometry.vertices.push(new THREE.Vector3(scaleView*AX,scaleView*AY, scaleView*AZ));
	
	var lineAxon = new THREE.Line(lineGeometry, sphereMaterial);

	scene.add( lineAxon );

	renderer.render(scene, camera);

	console.log("adding node to THREE");

}
function bindNodes([SourceX,SourceY,SourceZ], [TargetX,TargetY, TargetZ]){
	var lineGeometry = new THREE.Geometry();
	lineGeometry.vertices.push(new THREE.Vector3(scaleView*SourceX,scaleView*SourceY,scaleView*SourceZ));
	lineGeometry.vertices.push(new THREE.Vector3(scaleView*TargetX,scaleView*TargetY, scaleView*TargetZ));
	var lineMaterial = new THREE.MeshLambertMaterial(
	{
	    color: 0xFFFFFF
	});
	var line3 = new THREE.Line(lineGeometry, lineMaterial);
	scene.add(line3);

	renderer.render(scene, camera);

	console.log("adding bind to THREE");
}
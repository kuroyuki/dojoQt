var scene, renderer, camera, person, controls;

function setupDrawScene(){

	// set some camera attributes
	var VIEW_ANGLE = 45,
	    ASPECT = window.innerWidth / window.innerHeight
	    NEAR = 1,
	    FAR = 1000;

	// get the DOM element to attach to
	// - assume we've got jQuery to hand
	var $mainScene = $('#mainScene');

	// create a WebGL renderer, camera
	// and a scene
	renderer = new THREE.WebGLRenderer();
	
	// start the renderer
	renderer.setSize(window.innerWidth , window.innerHeight);

	// attach the render-supplied DOM element
	$mainScene.append(renderer.domElement);


	camera = new THREE.PerspectiveCamera(  VIEW_ANGLE,
	                                ASPECT,
	                                NEAR,
	                                FAR  );
	// the camera starts at 0,0,0 so pull it back
	camera.position.y = 7;
	camera.position.z = 20;

	controls = new THREE.OrbitControls( camera );
	controls.damping = 0.2;
	controls.addEventListener( 'change', render );

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

}
function render(){
	renderer.render(scene, camera);
}
function onWindowResize() {

	camera.aspect = window.innerWidth / window.innerHeight;
	camera.updateProjectionMatrix();

	renderer.setSize( window.innerWidth, window.innerHeight );

	render();
}

function animate() {

	requestAnimationFrame(animate);
	controls.update();

}
function addNode(pos, axon, size){
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
function bindNodes(source, target){
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

import * as api from '../api.js';
import * as auth from '../authorisation.js';

const sec = $('.js-section-start'),
      mower3D_perspectiveRotation = THREE.Math.degToRad(30);  // rotate 30⁰ to give better 3rd-person perspective

let renderer3D,
    camera3D,
    scene3D,
    mower3D,
    lawnMesh,
    leftWheel,
    rightWheel,
    obstacleLeftMech,
    obstacleFrontMech,
    obstacleRightMech,
    requestAnimationInstance,
    isModelAvailable3D = false;

function setLaunchMowerState() {
  api.selectState("LAUNCHING")
  .catch(error => {
    if (error.status === 401) {
      auth.showLogin().then(() => {
        setLaunchMowerState();
      });
    } else {
      console.error(e.message);
    }
  });
}

function setMowingState() {
  api.selectState("MOWING")
  .catch(error => {
    if (error.status === 401) {
      auth.showLogin().then(() => {
        setMowingState();
      });
    } else {
      console.error(e.message);
    }
  });
}

function setDockingState() {
  api.selectState("DOCKING")
  .catch(error => {
    if (error.status === 401) {
      auth.showLogin().then(() => {
        setDockingState();
      });
    } else {
      console.error(e.message);
    }
  });
}

function setStopState() {
  api.selectState("STOP")
  .catch(error => {
    if (error.status === 401) {
      auth.showLogin().then(() => {
        setStopState();
      });
    } else {
      console.error(e.message);
    }
  });
}

function disableButton(classname) {
  document.querySelector(classname).setAttribute("disabled", "disabled");
}

function enableButton(classname) {
  document.querySelector(classname).removeAttribute("disabled");
}

function toggleStateButtons() {

  if (liam.data.status.state) {
    switch (liam.data.status.state) {
      case 'DOCKED': {
        enableButton('.js-launching');
        disableButton('.js-docking');
        disableButton('.js-mowing');
        disableButton('.js-stop');
        break;
      }
      case 'LAUNCHING': {
        disableButton('.js-launching');
        enableButton('.js-docking');
        disableButton('.js-mowing');
        enableButton('.js-stop');
        break;
      }
      case 'MOWING': {
        disableButton('.js-launching');
        enableButton('.js-docking');
        disableButton('.js-mowing');
        enableButton('.js-stop');
        break;
      }
      case 'DOCKING': {
        disableButton('.js-launching');
        disableButton('.js-docking');
        enableButton('.js-mowing');
        enableButton('.js-stop');
        break;
      }
      case 'CHARGING': {
        enableButton('.js-launching');
        disableButton('.js-docking');
        disableButton('.js-mowing');
        disableButton('.js-stop');
        break;
      }
      case 'STUCK': {
        disableButton('.js-launching');
        enableButton('.js-docking');
        enableButton('.js-mowing');
        disableButton('.js-stop');
        break;
      }
      case 'FLIPPED': {
        disableButton('.js-launching');
        disableButton('.js-docking');
        disableButton('.js-mowing');
        disableButton('.js-stop');
        break;
      }
      case 'STOP': {
        disableButton('.js-launching');
        enableButton('.js-docking');
        enableButton('.js-mowing');
        disableButton('.js-stop');
        break;
      }
      case 'MANUAL': {
        disableButton('.js-launching');
        enableButton('.js-docking');
        enableButton('.js-mowing');
        enableButton('.js-stop');
        break;
      }
      case 'TEST': {
        disableButton('.js-launching');
        enableButton('.js-docking');
        enableButton('.js-mowing');
        enableButton('.js-stop');
        break;
      }
      default: {
        console.log('Unknown state: ' + liam.data.status.state);
      }
    }
  }
}

function updateBattery() {
  if (liam.data.status && liam.data.status.hasOwnProperty('batteryLevel')) {
    let batteryTicks = document.querySelectorAll('.js-battery .js-tick');
  
    if (liam.data.status.isCharging) {
      document.querySelector('.js-charging').style.visibility = 'visible';
      document.querySelector('.js-battery-value').style.visibility = 'hidden';

      batteryTicks.forEach(function(tick) {
        tick.style.fill = "#880";
        tick.classList.add('pulsate');
      });

    } else {
      document.querySelector('.js-charging').style.visibility = 'hidden';
      let batteryValueEl = document.querySelector('.js-battery-value');
      batteryValueEl.textContent = liam.data.status.batteryLevel + '%';
      batteryValueEl.style.visibility = 'visible';

      batteryTicks.forEach(function(tick) {
        tick.classList.remove('pulsate');
      });
      
      batteryTicks[0].style.fill = liam.data.status.batteryLevel >= 100 ? "#080" : "#b3b3b3";
      batteryTicks[1].style.fill = liam.data.status.batteryLevel > 90 ? "#080" : "#b3b3b3";
      batteryTicks[2].style.fill = liam.data.status.batteryLevel > 80 ? "#080" : "#b3b3b3";
      batteryTicks[3].style.fill = liam.data.status.batteryLevel > 70 ? "#080" : "#b3b3b3";
      batteryTicks[4].style.fill = liam.data.status.batteryLevel > 60 ? "#080" : "#b3b3b3";
      batteryTicks[5].style.fill = liam.data.status.batteryLevel >= 50 ? "#080" : "#b3b3b3";
  
      batteryTicks[6].style.fill = liam.data.status.batteryLevel >= 50 ? "#080" : liam.data.status.batteryLevel > 40 ? "#880" : "#b3b3b3";
      batteryTicks[7].style.fill = liam.data.status.batteryLevel >= 50 ? "#080" : liam.data.status.batteryLevel > 30 ? "#880" : "#b3b3b3";
  
      batteryTicks[8].style.fill = liam.data.status.batteryLevel >= 50 ? "#080" : liam.data.status.batteryLevel > 20 ? "#880" : liam.data.status.batteryLevel > 10 ? "#800" : "#b3b3b3";
      batteryTicks[9].style.fill = liam.data.status.batteryLevel >= 50 ? "#080" : liam.data.status.batteryLevel > 20 ? "#880" : liam.data.status.batteryLevel > 0 ? "#800" : "#b3b3b3";
    }
  }
}

function updatedStatus() {

  if (!liam.data.status) {
    return;
  }
  
  let text;
  switch (liam.data.status.state) {
    case 'DOCKED': text = 'DOCKED'; break;
    case 'LAUNCHING': text = 'LAUNCHING'; break;
    case 'MOWING': text = 'MOWING'; break;
    case 'DOCKING': text = 'DOCKING'; break;
    case 'CHARGING': text = 'CHARGING'; break;
    case 'STUCK': text = 'STUCK'; break;
    case 'FLIPPED': text = 'FLIPPED'; break;
    case 'MANUAL': text = 'MANUAL'; break;
    case 'STOP': text = 'STOPPED'; break;
    case 'TEST': text = 'TEST'; break;
    default: text = '...';
  }

  sec.find('.js-state').text(text);

  toggleStateButtons();
  updateBattery();
}

function initModel3D(canvas) {

  try {
    
    let canvasWidth = $('.js-main').width(),
        canvasHeight = canvasWidth;

    scene3D = new THREE.Scene();
    scene3D.background = new THREE.Color( 0x87CEEB );
    scene3D.fog = new THREE.Fog( 0xcce0ff, 100, 1500 );
        
    // Renderer
    renderer3D = new THREE.WebGLRenderer({
      antialias: true,
      canvas: canvas
    });

    renderer3D.setPixelRatio(window.devicePixelRatio);
    renderer3D.setSize(canvasWidth, canvasHeight);
    renderer3D.setClearColor(0xefefef, 1);
    renderer3D.gammaInput = true;
    renderer3D.gammaOutput = true;
    renderer3D.shadowMap.enabled = true;
    renderer3D.physicallyCorrectLights = true;

    // Camera
    camera3D = new THREE.PerspectiveCamera(45, canvasWidth / canvasHeight, 1, 1000);
    camera3D.position.set(0, 60, -150); // move camera back some distance, soo that we don't end up in the middle of the model
    // TODO: from above when close to obstacles.
    //camera3D.position.set(0, 120, -1);  // move camera back some distance, soo that we don't end up in the middle of the model

    camera3D.lookAt(scene3D.position);
    scene3D.add(camera3D);

    let ambientLight = new THREE.AmbientLight(0xffffff, 0.8);
    scene3D.add(ambientLight);

    let dirLight = new THREE.DirectionalLight(0xffffff, 1);
    dirLight.position.set(50, 200, 100);
    dirLight.castShadow = true;

    dirLight.shadow.camera.left = -40;
    dirLight.shadow.camera.right = 40;
    dirLight.shadow.camera.top = 40;
    dirLight.shadow.camera.bottom = -40;
    dirLight.shadow.camera.near = 3;
    dirLight.shadow.camera.far = camera3D.far;
    dirLight.shadow.mapSize.width = 512;
    dirLight.shadow.mapSize.height = 512;

    scene3D.add(dirLight);

    // use for debugging shadows
    //var helper = new THREE.CameraHelper(dirLight.shadow.camera);
    //scene3D.add(helper);

    // Grass lawn
    let textureLoader = new THREE.TextureLoader();
    let groundTexture = textureLoader.load( 'https://smart-home.rocks/liam/grasslight-big.jpg' );
    groundTexture.wrapS = groundTexture.wrapT = THREE.RepeatWrapping;
    groundTexture.repeat.set(50, 50);
    groundTexture.anisotropy = 16;
    let groundMaterial = new THREE.MeshLambertMaterial({ map: groundTexture });

    lawnMesh = new THREE.Mesh(new THREE.PlaneBufferGeometry(10000, 10000), groundMaterial);
    lawnMesh.position.y = 0;
    lawnMesh.rotation.x = - Math.PI / 2;
    lawnMesh.receiveShadow = true;
    scene3D.add(lawnMesh);

    // add visualization of ultrasound sensor detected obstacles (one for each sensor).
    obstacleLeftMech = new THREE.Mesh(new THREE.BoxGeometry(60, 40, 10), new THREE.MeshLambertMaterial({
      color: 0xcdcdcd,
    }));
    obstacleLeftMech.castShadow = true;
    obstacleLeftMech.visible = false;
    obstacleLeftMech.rotation.y = THREE.Math.degToRad(45);
    scene3D.add(obstacleLeftMech);

    obstacleFrontMech = new THREE.Mesh(new THREE.BoxGeometry(60, 40, 10), new THREE.MeshLambertMaterial({
      color: 0xcdcdcd,
    }));
    obstacleFrontMech.castShadow = true;
    obstacleFrontMech.visible = false;
    scene3D.add(obstacleFrontMech);

    obstacleRightMech = new THREE.Mesh(new THREE.BoxGeometry(60, 40, 10), new THREE.MeshLambertMaterial({
      color: 0xcdcdcd,
    }));
    obstacleRightMech.castShadow = true;
    obstacleRightMech.visible = false;
    obstacleRightMech.rotation.y = THREE.Math.degToRad(-45);
    scene3D.add(obstacleRightMech);

    // Enable to freely rotate 3D-scene with your mouse (also enable script in index.ejs).
    /*var controls = new THREE.OrbitControls( camera3D, renderer3D.domElement );
    controls.minDistance = 3;
    controls.maxDistance = 1000;
    controls.maxPolarAngle = (Math.PI / 2) - 0.1;*/

    // Loader for the mower
    const loader = new THREE.GLTFLoader();
    const dracoLoader = new THREE.DRACOLoader();

    THREE.DRACOLoader.setDecoderPath('https://smart-home.rocks/liam/');
    //dracoLoader.setVerbosity(1);  // Verbose console logging.
    loader.setDRACOLoader(dracoLoader);

    loader.load('https://smart-home.rocks/liam/3d_mower.glb?v=1',
      // called when model has been loaded
      function (gltf) {    
        document.querySelector('.js-model3D-loader').hidden = true;

        mower3D = gltf.scene;
        mower3D.position.set(0, 0, 1);
        //mower3D.rotation.set(mower3D_perspectiveRotation, THREE.Math.degToRad(180), 0); // align mower with grid and in front-facing direction
        mower3D.scale.set (100, 100, 100);
				mower3D.castShadow = true;
        mower3D.receiveShadow = true;
        mower3D.traverse(function(child) { 
          if (child.isMesh) {
            child.castShadow = true;
            //child.receiveShadow = true;
          }
        });

        leftWheel = mower3D.getObjectByName('wheel_left');
        rightWheel = mower3D.getObjectByName('wheel_right');

        scene3D.add(mower3D);

        isModelAvailable3D = true;
        drawModel3D();
      },
      // called when loading is in progresses
      function(xhr) {
        if ( xhr.lengthComputable ) {
          document.querySelector('.js-loadertext').textContent = `${Math.round(xhr.loaded / xhr.total * 100, 2)}% loaded...`;
        }  
      },
      // called when loading has errors
      function(error) {
        console.warn('Failed to load 3D model. Stacktrace: ' + error.stack);
        document.querySelector('.js-loadertext').textContent = 'failed to load model';
      }
    );
     
    window.addEventListener('resize', onWindowResize, false);
    
    function onWindowResize() {
      canvasWidth = $('.js-main').width();
      canvasHeight = canvasWidth;

      camera3D.aspect = canvasWidth / canvasHeight;
      camera3D.updateProjectionMatrix();
      renderer3D.setSize(canvasWidth, canvasHeight);
    }
  } catch (err) {
    console.warn( 'Failed to init 3D-model view, ignoring displaying 3D representation. Stacktrace: ' + err.stack);
  }
}

/**
 * Detaches the object from the parent and adds it back to the scene without moving in worldspace.
 * @param {*} child 
 * @param {*} parent 
 * @param {*} scene 
 */
function detachModel(child, parent, scene) {
  child.applyMatrix(parent.matrixWorld);
  parent.remove(child);
  parent.updateMatrixWorld();
  scene.add(child);
}

/**
 * Attaches the object to the parent without the moving the object in the worldspace.
 * @param {*} child 
 * @param {*} scene 
 * @param {*} parent 
 */
function attachModel(child, scene, parent) {
  child.applyMatrix(new THREE.Matrix4().getInverse(parent.matrixWorld));
  scene.remove(child);
  parent.add(child);
  parent.updateMatrixWorld();
}

function drawModel3D() {
  // update model here.
  mower3D.rotation.z = THREE.Math.degToRad(liam.data.status.roll);
  mower3D.rotation.x = THREE.Math.degToRad(liam.data.status.pitch);

  leftWheel.rotation.x += THREE.Math.degToRad(3 / 200 * liam.data.status.leftWheelSpd);
  rightWheel.rotation.x += THREE.Math.degToRad(3 / 200 * liam.data.status.rightWheelSpd);
  // rotate lawn to illustrate mower turning
  lawnMesh.rotation.z = THREE.Math.degToRad(liam.data.status.heading);
  lawnMesh.position.z -= (0.6 / 100) * liam.data.status.leftWheelSpd;

  // wrapp lawn soo that we could not escape world
  if (lawnMesh.position.z < -2500) {
    lawnMesh.position.z += 2500;
  } else if (lawnMesh.position.z > 2500) {
    lawnMesh.position.z -= 2500;
  }

  renderer3D.render(scene3D, camera3D);
  requestAnimationInstance = requestAnimationFrame(drawModel3D);
}

export function selected() {
  if (isModelAvailable3D) {
    drawModel3D();
  }
}

export function unselected() {
  window.cancelAnimationFrame(requestAnimationInstance);
}

export function init() {

  window.addEventListener('statusUpdated', updatedStatus);
  updatedStatus();

  sec.find('.js-launching').on('click', function() {
    if (!this.hasAttribute('disabled')) {
      setLaunchMowerState();
    }
  });
  sec.find('.js-mowing').on('click', function() {
    if (!this.hasAttribute('disabled')) {
      setMowingState();
    }
  });
  sec.find('.js-docking').on('click', function() {
    if (!this.hasAttribute('disabled')) {
      setDockingState();
    }
  });
  sec.find('.js-stop').on('click', function() {
    if (!this.hasAttribute('disabled')) {
      setStopState();
    }
  });

  let mower3dCanvas = document.querySelector('.js-model3D'); 
  initModel3D(mower3dCanvas);
}

import * as api from '../api.js';
import * as auth from '../authorisation.js';

const GRID_TURN_RATE = 0.5,
      sec = $('.js-section-start'),
      mower3D_perspectiveRotation = THREE.Math.degToRad(30);  // rotate 30⁰ to give better 3rd-person perspective

let renderer3D,
    camera3D,
    scene3D,
    grid3D,
    mower3D,
    leftWheel,
    rightWheel,
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
      document.querySelector('.js-charging').style.visibility = 'visibility';
      document.querySelector('.js-battery-value').style.display = 'none';

      batteryTicks.forEach(function(tick) {
        tick.style.fill = "#880";
        tick.classList.add('pulsate');
      });

    } else {
      document.querySelector('.js-charging').style.visibility = 'hidden';
      let batteryValueEl = document.querySelector('.js-battery-value');
      batteryValueEl.textContent = liam.data.status.batteryLevel + '%';
      batteryValueEl.style.display = 'inline-block';

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

    renderer3D = new THREE.WebGLRenderer({
      antialias: true,
      canvas: canvas
    });

    renderer3D.setPixelRatio(window.devicePixelRatio);
    renderer3D.setSize(canvasWidth, canvasHeight);
    renderer3D.setClearColor(0xefefef, 1);
    //renderer3D.shadowMap.enabled = true;
    renderer3D.gammaOutput = true;
    renderer3D.physicallyCorrectLights = true;

    //Camera
    camera3D = new THREE.PerspectiveCamera(45, canvasWidth / canvasHeight, 0.1, 500);
    camera3D.position.z = 12; // move camera back some distance, soo that we don't end up in the middle of the model 
    scene3D.add(camera3D);

    let ambientLight = new THREE.AmbientLight(0xcccccc, 0.8);
    ambientLight.name = "ambientLight"
    scene3D.add( ambientLight );

    let dirLight = new THREE.DirectionalLight(0xffffff, 2);
    dirLight.position.multiplyScalar(50);
    dirLight.name = "dirlight";
    dirLight.castShadow = true;
    camera3D.add(dirLight);

    grid3D = new THREE.GridHelper(50, 50, 0xFF4444, 0x404040);
    grid3D.rotation.x = mower3D_perspectiveRotation;
    scene3D.add(grid3D);

    //scene3D.fog = new THREE.Fog(0x72645b, 2, 100);

    //Loader for the model
    const loader = new THREE.GLTFLoader();
    const dracoLoader = new THREE.DRACOLoader();

    THREE.DRACOLoader.setDecoderPath('https://smart-home.rocks/liam/');
    //dracoLoader.setVerbosity(1);  // Verbose console logging.
    loader.setDRACOLoader(dracoLoader);

    loader.load('https://smart-home.rocks/liam/3d_mower.glb',
      // called when model has been loaded
      function (gltf) {    
        mower3D = gltf.scene;
        mower3D.position.set(0, 0, 1);
        mower3D.rotation.set(mower3D_perspectiveRotation, THREE.Math.degToRad(180), 0); // align mower with grid and in front-facing direction
        mower3D.scale.set (10, 10, 10);

        // TODO: parenting between wheel and tire should be done when exporting model, not here.
        leftWheel = mower3D.getObjectByName('wheel_left');
        rightWheel = mower3D.getObjectByName('wheel_right');

        let leftTire = mower3D.getObjectByName('tire_left');
        let rightTire = mower3D.getObjectByName('tire_right');

        attachModel(leftTire, mower3D, leftWheel);
        attachModel(rightTire, mower3D, rightWheel);
        // TODO: to get the tires back on the wheels. This should not need to be hardcoded.
        leftTire.position.set(38,0,0);
        rightTire.position.set(38,0,0);

        /*mower3D.traverse((o) => {
          if (o.isMesh) {
            //o.geometry.scale( -1, 1, 1 );
            console.log(o.name);
          }
        });*/

        scene3D.add(mower3D);

        isModelAvailable3D = true;
        drawModel3D();
      },
      // called when loading is in progresses
      function(xhr) {
        if ( xhr.lengthComputable ) {
          console.info(`3D model, ${Math.round(xhr.loaded / xhr.total * 100, 2)}% loaded`);
        }  
      },
      // called when loading has errors
      function(error) {    
        console.warn('Failed to load 3D model. Stacktrace: ' + error.stack);
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
  mower3D.rotation.x = mower3D_perspectiveRotation + THREE.Math.degToRad(liam.data.status.pitch);
  // animate grid? https://codepen.io/prisoner849/pen/LBWqpZ
  leftWheel.rotation.x += THREE.Math.degToRad(3 / 100 * liam.data.status.leftWheelSpd);
  rightWheel.rotation.x += THREE.Math.degToRad(3 / 100 * liam.data.status.rightWheelSpd);
  // rotate grid to illustrate mower turning
  let turnRate = liam.data.status.leftWheelSpd - liam.data.status.rightWheelSpd;
  grid3D.rotation.y += THREE.Math.degToRad(3 / 100 * turnRate * GRID_TURN_RATE);

  camera3D.lookAt(scene3D.position);
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

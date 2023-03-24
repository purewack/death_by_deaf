import "./styles.css"

import * as THREE from "three";
import levelData from "./level.json";
import { initControls, updateControls } from "./controls.js";
import { initScene } from "./scene.js";
import { initEditor } from "./editor.js";

function main() {
  const canvas = document.getElementById("app");
  canvas.width = 300;
  canvas.height = 200;

  initEditor("#editor");

  const renderer = new THREE.WebGLRenderer({ antialias: false, canvas });
  const fov = 45;
  const aspect = canvas.width / canvas.height; // the canvas default
  const near = 0.1;
  const far = 100;
  const camera = new THREE.PerspectiveCamera(fov, aspect, near, far);
  camera.position.z = 4;
  camera.position.y = 1;

  let player = {
    camera: null,
    direction: null,
    facing: [0, 0],
    mouse: { x: 0, y: canvas.height / 2 },
    oldMouse: { x: 0, y: canvas.height / 2 },
    active: false
  };
  player.camera = camera;

  initControls(player, canvas);

  const scene = new THREE.Scene();

  initScene(scene, levelData);

  {
    const color = 0xffffff;
    const intensity = 1;
    const light = new THREE.PointLight(color, intensity);
    light.position.set(0, 1.5, 0);
    const lightH = new THREE.PointLightHelper(light, light.intensity, "yellow");
    scene.add(light);
    scene.add(lightH);

    const grid = new THREE.PlaneGeometry(20, 20, 20, 20);
    const gridMat = new THREE.MeshBasicMaterial({
      color: "gray",
      wireframe: true
    });
    const gridMesh = new THREE.InstancedMesh(grid, gridMat, 6);
    grid.rotateX(Math.PI / 2);
    scene.add(gridMesh);

    const pos = new THREE.Object3D()

    pos.translateX(10).translateY(10).rotateZ(Math.PI/2).updateMatrix()
    gridMesh.setMatrixAt(1,pos.matrix)
    pos.translateX(10).translateY(10).rotateZ(Math.PI/2).updateMatrix()
    gridMesh.setMatrixAt(2,pos.matrix)
    pos.translateX(10).translateY(10).rotateZ(Math.PI/2).updateMatrix()
    gridMesh.setMatrixAt(3,pos.matrix)
    pos.translateZ(10).translateY(10).rotateX(Math.PI/2).updateMatrix()
    gridMesh.setMatrixAt(4,pos.matrix)
    pos.translateY(-20).updateMatrix()
    gridMesh.setMatrixAt(5,pos.matrix)

    gridMesh.setColorAt(0, new THREE.Color('gray'))
    gridMesh.setColorAt(1, new THREE.Color('red'))
    gridMesh.setColorAt(2, new THREE.Color('yellow'))
    gridMesh.setColorAt(3, new THREE.Color('green'))
    gridMesh.setColorAt(4, new THREE.Color('blue'))
    gridMesh.setColorAt(5, new THREE.Color('purple'))

    const axesHelper = new THREE.AxesHelper(20);
    scene.add(axesHelper);
  }

  function render(time) {
    // onCanvasResize(canvas);
    updateControls(canvas, player, time);
    time *= 0.001;

    renderer.render(scene, player.camera);

    requestAnimationFrame(render);
  }
  requestAnimationFrame(render);
}

main();

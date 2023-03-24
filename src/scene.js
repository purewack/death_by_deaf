import * as THREE from "three";
import { GLTFLoader } from 'three/examples/jsm/loaders/GLTFLoader.js';
import * as BufferGeometryUtils from "three/examples/jsm/utils/BufferGeometryUtils";

export function initScene(scene, data) {
  console.log(data);

  const loader = new GLTFLoader();
  loader.load(data.tiles.source, function (gltf) {
    console.log(gltf)
    const wall = gltf.scene.children[data.tiles.set.wall];
    const corner_in = gltf.scene.children[data.tiles.set.corner_in];
    const corner_out = gltf.scene.children[data.tiles.set.corner_out];
    const floor = gltf.scene.children[data.tiles.set.floor];

    const def_mat = new THREE.MeshPhongMaterial({ color: "pink" });
    const mat_wall = wall.children[0].material;

    let pieces = [];

    const dirToRot = {
      north: -Math.PI / 2,
      south: Math.PI / 2,
      west: 0,
      east: Math.PI
    };
    const elements = {
      wall,
      corner_in,
      corner_out,
      floor
    };

    for (let i = 0; i < data.map.length; i++) {
      const piece = data.map[i];
      const s = data.tiles.scale;
      pieces.push(
        elements[piece.type].children[0].geometry
          .clone()
          .scale(s, s, s)
          .rotateY(dirToRot[piece.facing])
          .translate(piece.position[0], piece.position[1], piece.position[2])
      );
    }

    const mergedPieces = BufferGeometryUtils.mergeBufferGeometries(pieces);
    const dungeon = new THREE.Mesh(mergedPieces, mat_wall);
    scene.add(dungeon);
    console.log(pieces);
    console.log(mergedPieces);

    const dungeonBox = new THREE.BoxHelper(dungeon, 0x00ff00);
    scene.add(dungeonBox);
  });
}

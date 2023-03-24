import * as THREE from "three";

export function initControls(player, target) {
  const keydown = (ev) => {
    if (ev.key === "w") player.direction = "forward";
    if (ev.key === "s") player.direction = "backward";
  };
  const keyup = (ev) => {
    if (ev.key === "w") player.direction = null;
    if (ev.key === "s") player.direction = null;
  };
  const pointermove = (event) => {
    if (!player.active) return;
    player.mouse.x += event.movementX;
    player.mouse.y += event.movementY;
  };

  document.addEventListener(
    "pointerlockchange",
    () => {
      if (document.pointerLockElement === target) {
        if (!player.active) {
          player.active = true;
          target.addEventListener("pointermove", pointermove);
          window.addEventListener("keydown", keydown);
          window.addEventListener("keyup", keyup);
          console.log("The pointer lock status is now locked");
        }
      } else {
        if (player.active) {
          player.active = false;
          target.removeEventListener("pointermove", pointermove);
          window.removeEventListener("keydown", keydown);
          window.removeEventListener("keyup", keyup);
          console.log("The pointer lock status is now unlocked");
        }
      }
    },
    false
  );

  target.addEventListener("click", async () => {
    await target.requestPointerLock();
  });
}

export function updateControls(canvas, player, dt) {
  const comp = getComputedStyle(canvas);
  const size = [parseFloat(comp.width), parseFloat(comp.height)];
  if (player.oldMouse === null) return;
  if (player.mouse === null) return;

  const aa = 0.1;
  const walk_speed = 0.04;

  const look = new THREE.Vector3();
  player.camera.getWorldDirection(look);

  let [px, py, pz] = player.camera.position.toArray();
  let [lx, ly, lz] = look.toArray();
  let [rx, ry] = player.facing;
  let mx = player.oldMouse.x;
  let my = player.oldMouse.y;
  let nx = player.mouse.x;
  let ny = player.mouse.y;

  if (player.direction === "forward") {
    pz = pz + walk_speed * Math.cos(rx);
    px = px + walk_speed * Math.sin(rx);
    //py = 1.5f + 0.04f*std::sin(dt*6.f) --head bop
  }
  if (player.direction === "backward") {
    pz = pz - walk_speed * 0.5 * Math.cos(rx);
    px = px - walk_speed * 0.5 * Math.sin(rx);
    //py = 1.5f + 0.04f*std::sin(dt*6.f) --head bop
  }

  lx = px + Math.sin(rx);
  lz = pz + Math.cos(rx);
  ly = py + ry;

  rx = (-mx / size[0] - 0.5) * Math.PI * 2 + Math.PI;
  ry = (my / size[1] - 0.5) * -Math.PI;

  mx = mx + (nx - mx) * aa;
  my = my + (ny - my) * aa;

  player.camera.position.set(px, py, pz);
  player.camera.lookAt(new THREE.Vector3(lx, ly, lz));
  player.facing = [rx, ry];
  player.oldMouse = { x: mx, y: my };
  player.screen = size;
}

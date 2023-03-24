import interact from "interactjs";

export function initEditor(element) {
  const pixelSize = 16;

  interact(element).draggable({
    max: Infinity,
    maxPerElement: Infinity,
    origin: "self",
    modifiers: [
      interact.modifiers.snap({
        // snap to the corners of a grid
        targets: [interact.snappers.grid({ x: pixelSize, y: pixelSize })]
      })
    ],
    listeners: {
      // draw colored squares on move
      move: function (event) {
        var context = event.target.getContext("2d");
        // calculate the angle of the drag direction
        var dragAngle = (180 * Math.atan2(event.dx, event.dy)) / Math.PI;

        // set color based on drag angle and speed
        context.fillStyle =
          "hsl(" +
          dragAngle +
          ", 86%, " +
          (30 + Math.min(event.speed / 1000, 1) * 50) +
          "%)";

        // draw squares
        context.fillRect(
          event.pageX - pixelSize / 2,
          event.pageY - pixelSize / 2,
          pixelSize,
          pixelSize
        );
      }
    }
  });

  function resizeCanvases() {
    [].forEach.call(document.querySelectorAll(element), function (canvas) {
      delete canvas.width;
      delete canvas.height;

      var rect = canvas.getBoundingClientRect();

      canvas.width = rect.width;
      canvas.height = rect.height;
    });
  }

  resizeCanvases();

  // interact.js can also add DOM event listeners
  interact(window).on("resize", resizeCanvases);
}

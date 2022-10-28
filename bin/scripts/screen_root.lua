visuals.floor(true)

vignette = visuals.addVImage("gfx/vig.png")
vignette.x = 0
vignette.y = 0
vignette.w = 800
vignette.h = 800
vignette.ax = 0.0
vignette.ay = 0.0


door1 = visuals.addVObject("gfx/door.obj")
door1.z = 10.0
door1.x = 2.0
door1.axis_y = 1.0
door1.axis_angle = 180

door2 = visuals.addVObject("gfx/door.obj")
door2.z = 10.0
door2.x = -2.0
door2.axis_y = 1.0
door2.axis_angle = 180

bb = visuals.addVButton()
bb.x = 10
bb.y = 10
bb.w = 30
bb.h = 30
bb.action = function()
    player.active = not player.active
    print(player.active)
end
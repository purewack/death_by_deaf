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


player.setPos(0,1.5,0)
player.setLook(0,1.5,0)
player.setRotation(0,0)
player.setFov(90)

colind = visuals.addVButton()
colind.x = S_W-50
colind.w = 50
colind.h = 50
colind.ay = 0
colind.ay = 0
colind.hue = 54
colind.state = false

col = false

system.onEngineTick = function(dt)
    cc = control.isPCollisionObj(door1)
    col = cc

end

m = false

visuals.onVideoFrame3D = function()
end

camera_filter = 0.1

visuals.onVideoFrame = function(dt)
    
    local aa = camera_filter --floatiness of camera
    local walk_speed = 0.04

    px,py,pz = player.getPos()
    lx,ly,lz = player.getLook()
    rx,ry = player.getRotation()
    mx,my,nx,ny = player.getMouse()
    
    if control.isKeyPressed(77) then 
        m = not m    
        visuals.mouse(m)
    end    

    if control.isKeyPressed(87) then
        audio.send_bang("walk_start")
        bb.state = true
    end
    
    if control.isKeyReleased(87)  then
        audio.send_bang("walk_stop")
        bb.state = false
    end

    if control.isKeyDown(87) then --W key
        pz = pz + walk_speed * math.cos(rx)
        px = px + walk_speed * math.sin(rx)
        -- py = 1.5f + 0.04f*std::sin(dt*6.f) --head bop
    end
    if control.isKeyDown(83) then --W key
        pz = pz - walk_speed*0.5 * math.cos(rx)
        px = px - walk_speed*0.5 * math.sin(rx)
        -- py = 1.5f + 0.04f*std::sin(dt*6.f) --head bop
    end
        
	-- 	-- if(IsKeyDown(KEY_A)) {
	-- 	--     puppet.rot.x += (2.0f/360.0f)*2.0f*3.1415f;
	-- 	-- }
	-- 	-- if(IsKeyDown(KEY_D)) {
	-- 	--     puppet.rot.x -= (2.0f/360.0f)*2.0f*3.1415f;
	-- 	-- }
    lx = px + math.sin(rx)
    lz = pz + math.cos(rx)
    ly = py + ry

    rx = ((mx/S_W) - 0.5)*-3.1415
    ry = ((my/S_H) - 0.5)*-3.1415

    mx = mx + (nx-mx)*aa
    my = my + (ny-my)*aa

    player.setPos(px,py,pz)
    player.setLook(lx,ly,lz)
    player.setRotation(rx,ry)
    player.setMouse(mx,my)
end

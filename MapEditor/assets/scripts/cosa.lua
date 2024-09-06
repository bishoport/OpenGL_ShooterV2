io.write("Cosa.LUA LOADED\n") 

local transform = nil

function Init()
    print("Lua Init function called")
    -- Obtener el transform desde el script
    transform = script:getTransform()
    if transform then
        local position = script:getPosition()
        position.y = position.y + 1
        script:setPosition(position)
        print("Initial Position: ", position.x, position.y, position.z)
    else
        print("Transform not found")
    end
end

function Update(deltaTime)
    if transform then
        local position = script:getPosition()
        position.y = position.y + 0.001
        script:setPosition(position)
    else
        print("Transform not found in Update")
    end
end

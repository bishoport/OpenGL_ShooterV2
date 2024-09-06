io.write("Cosa.LUA LOADED\n") 

local transform = nil

function Init()
    print("Lua Init function called")
    -- Obtener y mostrar el transform inicial
    transform = script:getTransform()
    if transform then
        local position = transform:getPosition()
        print("Initial Position: ", position.x, position.y, position.z)
    end
end

function Update(deltaTime)
    -- Verificar si el transform fue almacenado
    if transform then
        local position = transform:getPosition()
        --print("Current Position before update: ", position.x, position.y, position.z)
        
        -- Actualizar la posición y verificar el nuevo valor
        position.y = position.y + (deltaTime)
        transform:setPosition(position)

        -- Imprimir la nueva posición
        local newPosition = transform:getPosition()
        --print("Updated Position: ", newPosition.x, newPosition.y, newPosition.z)
    else
        print("Transform not found")
    end
end







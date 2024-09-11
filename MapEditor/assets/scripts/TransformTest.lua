function Init()
    print("Lua Init function called")
end

function Update(deltaTime)
    -- Obtener la posición actual
    local x, y, z = script:GetPosition()

    -- Verificar si los valores son válidos antes de establecer la nueva posición
    if x ~= nil and y ~= nil and z ~= nil then
        -- Incrementar la posición en el eje Y
        y = y + (1.0 * deltaTime)

        -- Establecer la nueva posición
        script:SetPosition(x, y, z)
    else
        print("Error: One of the position values is nil!")
    end
end

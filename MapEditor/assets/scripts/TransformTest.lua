function Init()
    print("Lua Init function called")
end

function Update(deltaTime)
    -- Obtener la posici�n actual
    local x, y, z = script:GetPosition()

    -- Verificar si los valores son v�lidos antes de establecer la nueva posici�n
    if x ~= nil and y ~= nil and z ~= nil then
        -- Incrementar la posici�n en el eje Y
        y = y + (1.0 * deltaTime)

        -- Establecer la nueva posici�n
        script:SetPosition(x, y, z)
    else
        print("Error: One of the position values is nil!")
    end
end

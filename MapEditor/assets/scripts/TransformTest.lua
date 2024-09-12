-- Tabla especial para las variables expuestas a ImGui
exposedVars = {
    speed = 1.0,     -- Velocidad de desplazamiento
    rotationSpeed = 30.0, -- Velocidad de rotación
    scale = 1.0,      -- Escala uniforme

    selectedModel = ""  -- Variable para almacenar el nombre del modelo
}


function Init()
    print("Lua Init function called")
    print(exposedVars.selectedModel)

    script:InstanceModel(exposedVars.selectedModel)
end

function Update(deltaTime)
    -- Usar las variables expuestas
    local x, y, z = script:GetPosition()
    y = y + (exposedVars.speed * deltaTime)  -- Desplazarse usando la velocidad desde ImGui
    script:SetPosition(x, y, z)

    -- Rotar el objeto usando rotationSpeed
    local rx, ry, rz = script:GetRotation()
    ry = ry + (exposedVars.rotationSpeed * deltaTime)
    script:SetRotation(rx, ry, rz)

    -- Escalar el objeto usando scale
    local sx, sy, sz = exposedVars.scale, exposedVars.scale, exposedVars.scale
    script:SetScale(sx, sy, sz)
end


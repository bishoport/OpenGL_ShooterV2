-- Tabla especial para las variables expuestas a ImGui
exposedVars = {
    speed = 1.0,          -- Velocidad de desplazamiento
    rotationSpeed = 30.0, -- Velocidad de rotaci�n
    scale = 1.0,          -- Escala uniforme
    selectedModel = ""    -- Variable para almacenar el nombre del modelo
}

local modelEntity

function Init()
    print("Lua Init function called")
    print(exposedVars.selectedModel)
    
    -- Crear la entidad a partir del modelo seleccionado
    modelEntity = EntityManager:CreateEntityFromModel(exposedVars.selectedModel)
end

function Update(deltaTime)
    -- Usar las variables expuestas
    
    -- Obtener la posici�n actual del objeto
    local x, y, z = EntityManager:GetPosition(modelEntity)
    -- Modificar la posici�n en el eje Y seg�n la velocidad
    y = y + (exposedVars.speed * deltaTime)
    -- Establecer la nueva posici�n
    EntityManager:SetPosition(modelEntity, x, y, z)

    -- Rotar el objeto usando rotationSpeed
    local rx, ry, rz = EntityManager:GetRotation(modelEntity)
    ry = ry + (exposedVars.rotationSpeed * deltaTime)
    EntityManager:SetRotation(modelEntity, rx, ry, rz)

    -- Escalar el objeto usando scale
    local sx, sy, sz = exposedVars.scale, exposedVars.scale, exposedVars.scale
    EntityManager:SetScale(modelEntity, sx, sy, sz)
end

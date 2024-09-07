--io.write("Cosa.LUA LOADED\n")

function Init()
    print("Lua Init function called")

    -- Obtener y modificar el valor de simpleInt en Init
    local currentValue = script:getSimpleInt()
    print("Initial simpleInt value in Init: ", currentValue)
    
    -- Incrementar simpleInt por 10
    script:setSimpleInt(currentValue + 10)
    
    local updatedValue = script:getSimpleInt()
    print("Updated simpleInt value in Init: ", updatedValue)
end

function Update(deltaTime)
    print("Calling Lua Update function")

    -- Leer el valor de simpleInt en cada frame
    local currentValue = script:getSimpleInt()
    print("Current simpleInt value in Update: ", currentValue)

    -- Incrementar simpleInt en cada frame
    script:setSimpleInt(currentValue + 1)
    
    local updatedValue = script:getSimpleInt()
    print("Updated simpleInt value in Update: ", updatedValue)
end

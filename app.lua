conn = net.createUDPSocket()
gpio.mode(1, gpio.INT, gpio.PULLUP)
count = 0
function fn(level, when, missed)
    if level == 1 then
        conn:send(11222, wifi.sta.getbroadcast(), "minmon " .. count)
        count = count + 1
        tmr.delay(100000)
    end
end

gpio.trig(1, "both", fn)


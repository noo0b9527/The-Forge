loader.SetRenderMode(2)
loader.SetCounter(5)

local selectedAPIIndex = 0
if loader.GetSelectAPI ~= nil then
	selectedAPIIndex = loader.GetSelectAPI()
end

if loader.RequestScreenshotCapture ~= nil then
	loader.RequestScreenshotCapture("API" .. selectedAPIIndex .. "_" .. "WaveIsFirstLane")
end
$timestamp = Get-Date -Format "MM-dd HH:mm:ss.fff"
adb logcat -T "$timestamp" main-modloader:W QuestHook[qounters-minus`|v0.2.3]:* AndroidRuntime:E *:S

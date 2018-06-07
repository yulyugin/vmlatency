@SET driver="%~dp0build-win7-fre\amd64\vmlatency.sys"

SC CREATE vmlatency binPath= %driver% type= kernel
SC START vmlatency
SC STOP vmlatency > nul 2>&1
SC DELETE vmlatency > nul 2>&1

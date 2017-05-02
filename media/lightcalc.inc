    vec4 col;
    
    // calculate the normal in view space
    vec3 normal = normalize(matNormal*aNormal);
    col  = uLight1Col * max(dot(normal,uLight1Dir),0.0);
    col += uLight2Col * max(dot(normal,uLight2Dir),0.0);
    col += uLight3Col * max(dot(normal,uLight3Dir),0.0);
    vCol = colDiffuse * col + uAmbient;
    
    vCol = min(vCol,1.0);
    
    float eyedist = gl_Position.z;
    vFog = smoothstep(uFogDist[0],uFogDist[1],eyedist);


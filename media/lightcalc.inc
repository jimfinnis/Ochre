    vec4 col;
    
    // calculate the normal in view space
    // have to normalize because we do scaling
    
    vec3 normalnorm = normalize(matNormal*aNormal);
    vec3 normal = matNormal*aNormal;
    
    col  = uLight1Col * max(dot(normalnorm,uLight1Dir),0.0);
    col += uLight2Col * max(dot(normalnorm,uLight2Dir),0.0);
    col += uLight3Col * max(dot(normalnorm,uLight3Dir),0.0);
    
    vCol = colDiffuse * col + uAmbient;
    
    
    vCol = min(vCol,1.0);
    vCol.w = colDiffuse.w;
    
    float eyedist = gl_Position.z;
    vFog = smoothstep(uFogDist[0],uFogDist[1],eyedist);

    

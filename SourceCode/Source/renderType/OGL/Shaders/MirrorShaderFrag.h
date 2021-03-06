static char effectMirrorShaderFrag[] = { 
"//#define MaxChannels 3\n" 
"// float MirrorRadius = .1;\n" 
"//vec3 MirrorCenter = vec3(0,0,0); // Mirror Center\n" 
"//float Grace = 1.0;\n" 
"//vec3 ProjectorOrigin= vec3(0,0,-.99); // Projector Origin\n" 
"//vec3 ProjectorRayUp = vec3( 0,1,0);\n" 
"//vec3 AudiencePosition = vec3(0,0,0);\n" 
"float DomeRadius = 1.0;\n" 
"vec3 DomeCenter = vec3(0,0,0); // dome Center\n" 
"// The images from the seperate RenderChannels\n" 
"// For this hack, order assummed to be left, right, top.\n" 
"//uniform sampler2D channelTextures[MaxChannels];	\n" 
"uniform sampler2D channelTexture0;\n" 
"uniform sampler2D channelTexture1;\n" 
"uniform sampler2D channelTexture2;\n" 
"uniform sampler2D channelTexture3;\n" 
"uniform bool stencilMaskEnabled;\n" 
"uniform bool debug1;\n" 
"uniform sampler2D StencilMask;\n" 
"varying vec3 VertexPosition_WorldSpace;\n" 
"varying float R;\n" 
"varying vec2 PixelPos;\n" 
"uniform bool useTexture;\n" 
"float RaySphereIntersection(vec3 o, vec3 d, vec3 c, float R ) {\n" 
"	vec3 oc = o-c;\n" 
"	float discriminant = dot(d,oc)*dot(d,oc) - dot(d,d) * (dot(oc,oc)-R*R);\n" 
"	\n" 
"	// assume D>0 and there are 2 hits.\n" 
"	float PlusT = (dot(-d,oc) + sqrt(discriminant) ) / dot(d,d);\n" 
"	float NegT = (dot(-d,oc) - sqrt(discriminant) ) / dot(d,d);\n" 
"	if(discriminant <= 0.0)\n" 
"		return 0.0;\n" 
"	else\n" 
"	{\n" 
"		if(NegT > 0.0)\n" 
"			return NegT;\n" 
"		else\n" 
"			return PlusT;\n" 
"	}\n" 
"}\n" 
"vec4 ApplyMirrorTest(float4x4 texturematrix, sampler2D channelTexture,vec4 ViewRay, vec4 inColor)\n" 
"{\n" 
"		vec4 leftCoordProj 	= texturematrix * ViewRay;\n" 
"		vec2 leftTexCoords 	= leftCoordProj.xy / leftCoordProj.w;\n" 
"		if (leftCoordProj.w >= 0.0 && leftTexCoords.x >= 0.0 && leftTexCoords.x <= 1.0 && leftTexCoords.y >= 0.0 && leftTexCoords.y <= 1.0)\n" 
"			inColor = texture2D(channelTexture, vec2(leftTexCoords.x, leftTexCoords.y));\n" 
"        return inColor;\n" 
"}\n" 
"	\n" 
"void main()  {\n" 
"	vec3 ProjectorRayForward = normalize(MirrorCenter-ProjectorOrigin);\n" 
"	vec3 ProjectorRayRight = normalize(cross(ProjectorRayForward,ProjectorRayUp));\n" 
"	ProjectorRayUp =	normalize(cross(ProjectorRayRight ,ProjectorRayForward));\n" 
"	// Ray Direction\n" 
"	vec3 d  = normalize(  MirrorCenter + \n" 
"									ProjectorRayUp   * Grace * mix(-MirrorRadius,MirrorRadius,gl_TexCoord[0].y)+\n" 
"									ProjectorRayRight * Grace * mix(-MirrorRadius,MirrorRadius,gl_TexCoord[0].x)\n" 
"									-ProjectorOrigin\n" 
"									);\n" 
"	float t = RaySphereIntersection(ProjectorOrigin,d,MirrorCenter,MirrorRadius);\n" 
"	vec3 p = ProjectorOrigin+t*d;\n" 
"	vec3 n = (p-MirrorCenter)/ MirrorRadius;\n" 
"	n= normalize(n);\n" 
"	\n" 
"	vec3 oDome = p; // Point on the mirror\n" 
"	vec3 dDome = reflect(d,n);// Reflect the d ray off of n\n" 
"		\n" 
"	dDome= normalize(dDome);\n" 
"	float   tDome = RaySphereIntersection(oDome,dDome,DomeCenter,DomeRadius);\n" 
"	vec3  pDome = oDome+tDome*dDome;\n" 
"	vec4 ViewRay = vec4(normalize(pDome-AudiencePosition),1);\n" 
"	vec4 outColor = vec4(0.0,0.0,0.0,1.0);\n" 
"	\n" 
"	\n" 
"	outColor = ApplyMirrorTest(gl_TextureMatrix[0], channelTexture0, ViewRay, outColor);\n" 
"	outColor = ApplyMirrorTest(gl_TextureMatrix[1], channelTexture1, ViewRay, outColor);\n" 
"	outColor = ApplyMirrorTest(gl_TextureMatrix[2], channelTexture2, ViewRay, outColor);\n" 
"	\n" 
"	if(MaxChannels>=4)\n" 
"		outColor = ApplyMirrorTest(gl_TextureMatrix[3], channelTexture3, ViewRay, outColor);\n" 
"	\n" 
"	/*\n" 
"	for(int i = 0; i < MaxChannels; i++) {\n" 
"		if (i == 0) {\n" 
"			vec4 leftCoordProj 	= gl_TextureMatrix[i] * ViewRay;\n" 
"			vec2 leftTexCoords 	= leftCoordProj.xy / leftCoordProj.w;\n" 
"			vec4 outColorLeft = texture2D(channelTexture0, vec2(leftTexCoords.x, leftTexCoords.y));\n" 
"			if (leftCoordProj.w >= 0.0&&leftTexCoords.x >= 0.0 && leftTexCoords.x <= 1.0 && leftTexCoords.y >= 0.0 && leftTexCoords.y <= 1.0)\n" 
"				outColor = outColorLeft;\n" 
"		} else if (i == 1) {\n" 
"			vec4 rightCoordProj	= gl_TextureMatrix[i] * ViewRay;\n" 
"			vec2 rightTexCoords = rightCoordProj.xy / rightCoordProj.w;\n" 
"			vec4 outColorRight = texture2D(channelTexture1, vec2(rightTexCoords.x, rightTexCoords.y));\n" 
"			if (rightCoordProj.w >= 0.0&&rightTexCoords.x >= 0.0 && rightTexCoords.x <= 1.0 && rightTexCoords.y >= 0.0 && rightTexCoords.y <= 1.0)\n" 
"				outColor = outColorRight;\n" 
"		} else if (i ==2) {\n" 
"			vec4 topCoordProj 	= gl_TextureMatrix[i] * ViewRay;\n" 
"			vec2 topTexCoords 	= topCoordProj.xy / topCoordProj.w;\n" 
"			vec4 outColorTop = texture2D(channelTexture2, vec2(topTexCoords.x, topTexCoords.y));\n" 
"			if (topCoordProj.w >= 0.0&& topTexCoords.x >= 0.0 && topTexCoords.x <= 1.0 && topTexCoords.y >= 0.0 && topTexCoords.y <= 1.0)\n" 
"				outColor = outColorTop;\n" 
"		} else if (i == 3) {\n" 
"			vec4 bottomCoordProj = gl_TextureMatrix[i] * ViewRay;\n" 
"			vec2 bottomTexCoords 	= bottomCoordProj.xy / bottomCoordProj.w;\n" 
"			vec4 outColorBottom = texture2D(channelTexture3, vec2(bottomTexCoords.x, bottomTexCoords.y));\n" 
"			if (bottomCoordProj.w >= 0.0&& bottomCoordProj.x >= 0.0 && bottomCoordProj.x <= 1.0 && bottomCoordProj.y >= 0.0 && bottomCoordProj.y <= 1.0)\n" 
"				outColor = outColorBottom;\n" 
"		}\n" 
"	}\n" 
"	*/\n" 
"	if (debug1)\n" 
"		gl_FragColor.xyz = outColor.xyz;		\n" 
"	else if (stencilMaskEnabled)\n" 
"		gl_FragColor.xyz = outColor.xyz * texture2D(StencilMask,PixelPos.xy).xyz;\n" 
"	else\n" 
"		gl_FragColor.xyz = outColor.xyz;\n" 
"}\n" 
}; 

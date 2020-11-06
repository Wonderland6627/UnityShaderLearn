// Upgrade NOTE: replaced 'mul(UNITY_MATRIX_MVP,*)' with 'UnityObjectToClipPos(*)'

// Upgrade NOTE: replaced 'mul(UNITY_MATRIX_MVP,*)' with 'UnityObjectToClipPos(*)'

Shader "Wonderland6627/ImageShaderPro"
{
	Properties
	{
		_MainTex ("贴图", 2D) = "white" {}
	}
	SubShader
	{
		Cull Off ZWrite Off ZTest Always

		Pass
		{
			CGPROGRAM	
			#pragma vertex vert	
			//指定顶点Shader入口
			#pragma fragment frag
			//指定片段程序入口

			#include "UnityCG.cginc"
			//引用Unity内置的一些定义

			struct v2f
			{
				float4 pos : POSITION;
				float4 col : COLOR;
			};

			v2f vert(appdata_base v)
			{
				v2f o;
				float angle = length(v.vertex) * _SinTime.w;
				float4x4 RM = 
				{
					float4(cos(angle),0,sin(angle),0),
					float4(0,1,0,0),
					float4(-1*sin(angle),0,cos(angle),0),
					float4(0,0,0,1)
				};
				float4 pos = mul(RM,v.vertex);
				o.pos = UnityObjectToClipPos(pos);

				angle = abs(cos(length(v.vertex)));
				o.col = float4(angle,1,0,1);

				return o;
			}

			float4 frag(v2f v) : COLOR
			{
				return v.col;
			}

			ENDCG
		}
	}
}

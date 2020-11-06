// Upgrade NOTE: replaced 'mul(UNITY_MATRIX_MVP,*)' with 'UnityObjectToClipPos(*)'

// Upgrade NOTE: replaced 'mul(UNITY_MATRIX_MVP,*)' with 'UnityObjectToClipPos(*)'

Shader "Wonderland6627/ImageShader"
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

			struct appdata
			{
				float4 vertex : POSITION;
				float2 uv : TEXCOORD;
			};

			struct v2f
			{
				float4 vertex : SV_POSITION;
				float2 uv : TEXCOORD0;
			};

			v2f vert(appdata v)
			{
				v2f o;
				o.vertex = UnityObjectToClipPos(v.vertex);
				o.uv = v.uv;

				return o;
			}

			sampler2D _MainTex;

			fixed4 frag(v2f i) : SV_TARGET
			{
				fixed4 col = tex2D(_MainTex,i.uv);
				//col = 1 - col;

				return col;
			}

			ENDCG
		}
	}
}

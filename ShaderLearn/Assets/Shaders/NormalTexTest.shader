Shader "Unlit/NormalTexTest"
{
	Properties
	{
		_Color ("颜色", Color) = (1,1,1,1)
		_MainTex ("原贴图", 2D) = "white" {}
		_NormalTex ("法线贴图", 2D) = "white"{}
	}
	SubShader
	{
		Tags { "RenderType"="Opaque" }
		LOD 100

		Pass
		{
			CGPROGRAM
			#pragma vertex vert
			#pragma fragment frag
			
			#include "UnityCG.cginc"

			float4 _Color;
			sampler2D _MainTex;
			float4 _NormalTex;

			/*struct appdata
			{
				float4 vertex : POSITION;
				float2 uv : TEXCOORD0;
				float4 col : COLOR;
			};*/

			struct v2f
			{
				float4 pos : SV_POSITION;
				float2 uv : TEXCOORD;
				float4 col : COLOR;
			};
	
			v2f vert (appdata_base v)
			{
				v2f o;
				o.pos = UnityObjectToClipPos(v.vertex);
				o.uv = v.texcoord;

				return o;
			}
			
			fixed4 frag (v2f i) : COLOR
			{
				return i.col;
			}

			ENDCG
		}
	}
}

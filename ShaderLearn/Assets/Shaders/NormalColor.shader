Shader "Unlit/NormalColor"
{
	Properties
	{
		_Color("颜色", Color) = (1,1,1,1)
	}
	SubShader
	{
		Tags
		{
			
		}

		Pass
		{
			CGPROGRAM
			#pragma vertex vert
			#pragma fragment frag

			#include "UnityCG.cginc"

			fixed4 _Color;

			struct v2f
			{
				float4 pos : SV_POSITION;
				float2 uv : TEXCOORD;
				float4 col : COLOR;
			};

			v2f vert(appdata_base v)
			{
				v2f o;
				o.pos = UnityObjectToClipPos(v.vertex);
				//o.uv = v.texcoord;
				o.col.rgb = v.normal*0.5 + 0.5;
				o.col.a = 0.1;

				return o;
			}

			float4 frag(v2f i) : COLOR
			{
				return i.col;
			}

			ENDCG
		}
	}
}

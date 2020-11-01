Shader "Wonderland6627/VFaceShader"
{
	Properties
	{
		_FrontTex("FrontTex",2d) = "white"{}
		_BackTex("BackTex",2d) = "white"{}
	}

	SubShader
	{
		cull off
		Pass
		{
			CGPROGRAM
			#pragma vertex vert
			#pragma fragment frag
			#pragma target 3.0

			sampler2D _FrontTex;
			sampler2D _BackTex;

			struct appdata
			{
				float4 vertex : POSITION;
				float2 texcoord : TEXCOORD0;
			};

			struct v2f
			{
				float4 pos : SV_POSITION;
				float2 uv : TEXCOORD0;
			};

			v2f vert(appdata v)
			{
				v2f o;
				o.pos = UnityObjectToClipPos(v.vertex);
				o.uv = v.texcoord;
				return o;
			}

			fixed4 frag (v2f i, float face : VFACE) : SV_Target
			{
				fixed4 col = 1;
				col = face > 0 ? tex2D(_FrontTex, i.uv) : tex2D(_BackTex, i.uv);
				return col;
			}
			
			ENDCG
		}
	}
}

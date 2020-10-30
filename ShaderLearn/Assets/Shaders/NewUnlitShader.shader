Shader "Wonderland6627/MyFirstShader"
{
	Properties
	{
		_Color("我是Color",Color) = (1,1,1,1)
		_Int("我是Int",Int) = 1
		_Float("我是Float",Float) = 0.5
		_FloatRange("我是Float滑动条",Range(0,1)) = 0.5
		[PowerSlider(3)]_FloatPower("我是FloatPower滑动条",Range(0,1)) = 0.5
		[IntRange]_FloatInt("我是FloatInt滑动条",Range(0,5)) = 1
		[Toggle]_Toggle("我是Toggle",Range(0,1)) = 1
		[Enum(UnityEngine.Rendering.CullMode)]	_Enum("我是Enum",Float)=1
		_Vector("我是Vector",Vector)=(0,0,0,0)
		_Main2DTex("我是2D纹理",2D) = "White"{}
		_Main3DTex("我是3D纹理",3D) = ""{}
		_MainCubeTex("我是Cube纹理",CUBE) = ""{}
	}

	SubShader
	{
		Pass
		{
			CGPROGRAM
			#pragma vertex vert
			#pragma fragment frag

			fixed4 _Color;

			struct appdata
			{
				float4 vertex : POSITION;
				float2 uv : TEXCOORD;
			};

			struct v2f
			{
				float4 pos : SV_POSITION;
				float2 uv : TEXCOORD;
			};

			v2f vert(appdata v)
			{
				v2f o;
				o.pos = UnityObjectToClipPos(v.vertex);
				o.uv = v.uv;
				return o;
			}

			fixed checker(float2 uv)
			{
				float2 repeatUV = uv * 10;
				float2 c = floor(repeatUV) / 2;
				float checker = frac(c.x + c.y) * 2;

				return checker;
			}

			float4 frag(v2f i) : SV_TARGET
			{
				fixed col = checker(i.uv);
				return col;
			}

			ENDCG
		}
	}

	FallBack "Diffuse"
	CustomEditor "EditorName"
}

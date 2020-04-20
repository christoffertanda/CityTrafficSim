#include "direct3d.h"

HWND	direct3d::WindowHandle = nullptr;
real32	direct3d::BufferWidth = 0.0f;
real32	direct3d::BufferHeight = 0.0f;

ID3D11Device			*direct3d::Device = nullptr;
IDXGISwapChain			*direct3d::Swap = nullptr;
ID3D11DeviceContext		*direct3d::Context = nullptr;
ID3D11RenderTargetView  *direct3d::Target = nullptr;

ID3D11DepthStencilState	*direct3d::DepthStencilState;
ID3D11DepthStencilView	*direct3d::DepthStencilView;
ID3D11BlendState		*direct3d::AlphaBlendState;
ID3D11Texture2D			*direct3d::DepthStencilBuffer;
D3D11_VIEWPORT			 direct3d::Viewport;

IDXGIDebug				*direct3d::Debug;

void direct3d::Init(HWND WindowHandle)
{
	direct3d::WindowHandle = WindowHandle;

	direct3d::BufferWidth = 1920.0f;
	direct3d::BufferHeight = 1080.0f;

	SetDevice();
	SetFrameBuffer();
	SetAlphaBlender();
	SetDepthStencil();
	SetViewport();
}

void direct3d::SetDevice()
{
	// NOTE(Cristoffer): Sets up the device parent together with swap chain buffer.

	HRESULT HR = S_OK;

	D3D_FEATURE_LEVEL FeatureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_1
	};

	uint32 CreationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

	#if DX_DEBUG

	CreationFlags |= D3D11_CREATE_DEVICE_DEBUG;

	#endif

	DXGI_SWAP_CHAIN_DESC DeviceDescription = {};

	DeviceDescription.BufferCount = 1;
	DeviceDescription.BufferDesc.Width = (uint32)BufferWidth;
	DeviceDescription.BufferDesc.Height = (uint32)BufferHeight;
	DeviceDescription.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	DeviceDescription.BufferDesc.RefreshRate.Numerator = 60;
	DeviceDescription.BufferDesc.RefreshRate.Denominator = 1;
	DeviceDescription.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	DeviceDescription.OutputWindow = WindowHandle;
	DeviceDescription.SampleDesc.Count = 1;
	DeviceDescription.SampleDesc.Quality = 0;
	DeviceDescription.Windowed = TRUE;

	HR = D3D11CreateDeviceAndSwapChain
	(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		CreationFlags,
		FeatureLevels,
		ARRAYSIZE(FeatureLevels),
		D3D11_SDK_VERSION,
		&DeviceDescription,
		&Swap,
		&Device,
		nullptr,
		&Context
	);

	D3D_ERROR_CHECK(HR);

	#if DX_DEBUG
	HR = Device->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void **>(&Debug));
	D3D_ERROR_CHECK(HR);
	#endif
}

void direct3d::SetFrameBuffer()
{
	// TODO(Cristoffer): Create the buffer and render target. Only single render target
	// for now. Multiple render targets needed in future?

	HRESULT HR = S_OK;

	ID3D11Texture2D *FrameBuffer;

	Swap->GetBuffer(0, __uuidof(ID3D11Texture2D), (void **)&FrameBuffer);

	HR = Device->CreateRenderTargetView(FrameBuffer, nullptr, &Target);

	D3D_ERROR_CHECK(HR);

	FrameBuffer->Release();
}

void direct3d::SetDepthStencil()
{
	// NOTE(Cristoffer): Depth stencil or "Z-buffer" for sorting pixels based on depth
	// instead of the order objects get drawn in.

	HRESULT HR = S_OK;

	D3D11_TEXTURE2D_DESC DepthStencilDesc;
	DepthStencilDesc.Width = (uint32)BufferWidth;
	DepthStencilDesc.Height = (uint32)BufferHeight;
	DepthStencilDesc.MipLevels = 1;
	DepthStencilDesc.ArraySize = 1;
	DepthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	DepthStencilDesc.SampleDesc.Count = 1;
	DepthStencilDesc.SampleDesc.Quality = 0;
	DepthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	DepthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	DepthStencilDesc.CPUAccessFlags = 0;
	DepthStencilDesc.MiscFlags = 0;

	HR = Device->CreateTexture2D(&DepthStencilDesc, NULL, &DepthStencilBuffer);
	D3D_ERROR_CHECK(HR);

	HR = Device->CreateDepthStencilView(DepthStencilBuffer, NULL, &DepthStencilView);
	D3D_ERROR_CHECK(HR);

	D3D11_DEPTH_STENCIL_DESC StencilDesc = {};
	StencilDesc.DepthEnable = true;
	StencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	StencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

	HR = Device->CreateDepthStencilState(&StencilDesc, &DepthStencilState);
	D3D_ERROR_CHECK(HR);
}

void direct3d::SetAlphaBlender()
{
	// NOTE(Cristoffer): Pixel alpha blending to handle transparency.

	HRESULT HR = S_OK;

	D3D11_RENDER_TARGET_BLEND_DESC RenderTargetBlendDesc = {};

	RenderTargetBlendDesc.BlendEnable = true;
	RenderTargetBlendDesc.SrcBlend = D3D11_BLEND_SRC_ALPHA;
	RenderTargetBlendDesc.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	RenderTargetBlendDesc.BlendOp = D3D11_BLEND_OP_ADD;
	RenderTargetBlendDesc.SrcBlendAlpha = D3D11_BLEND_ONE;
	RenderTargetBlendDesc.DestBlendAlpha = D3D11_BLEND_ZERO;
	RenderTargetBlendDesc.BlendOpAlpha = D3D11_BLEND_OP_ADD;
	RenderTargetBlendDesc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	D3D11_BLEND_DESC BlenderDesc = {};

	BlenderDesc.AlphaToCoverageEnable = false;
	BlenderDesc.IndependentBlendEnable = false;
	BlenderDesc.RenderTarget[0] = RenderTargetBlendDesc;

	HR = Device->CreateBlendState(&BlenderDesc, &AlphaBlendState);

	D3D_ERROR_CHECK(HR);
}

void direct3d::SetViewport()
{
	Viewport.Width = BufferWidth;
	Viewport.Height = BufferHeight;
	Viewport.MinDepth = 0.0f;
	Viewport.MaxDepth = 1.0f;
	Viewport.TopLeftX = 0.0f;
	Viewport.TopLeftY = 0.0f;
}

void direct3d::BeginFrame()
{
	// TODO(Cristoffer): Might need different setups and topologies depending
	// on what is rendered in future.

	real32 Color[4] = { 0.05f, 0.05f, 0.08f, 1.0f };

	Context->RSSetViewports(1, &Viewport);

	Context->ClearRenderTargetView(Target, Color);
	Context->ClearDepthStencilView(DepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	Context->OMSetRenderTargets(1, &Target, DepthStencilView);
	Context->OMSetDepthStencilState(DepthStencilState, 1);
	Context->OMSetBlendState(AlphaBlendState, 0, 0xffffffff);

	Context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void direct3d::EndFrame()
{
	// NOTE(Cristoffer): Present new buffer to screen.
	Swap->Present(0, 0);
}

real32 direct3d::GetBufferWidth()
{
	return BufferWidth;
}

real32 direct3d::GetBufferHeight()
{
	return BufferHeight;
}

ID3D11Device *direct3d::GetDevice()
{
	return Device;
}

ID3D11Texture2D *direct3d::GetDepthStencilBuffer()
{
	return DepthStencilBuffer;
}

ID3D11DepthStencilState *direct3d::GetDepthStencilState()
{
	return DepthStencilState;
}

ID3D11DepthStencilView *direct3d::GetDepthStencilView()
{
	return DepthStencilView;
}

ID3D11BlendState *direct3d::GetAlphaBlendState()
{
	return AlphaBlendState;
}

IDXGISwapChain *direct3d::GetSwap()
{
	return Swap;
}

ID3D11DeviceContext *direct3d::GetContext()
{
	return Context;
}

ID3D11RenderTargetView *direct3d::GetTarget()
{
	return Target;
}

direct3d::~direct3d()
{
	if(Context != nullptr)
	{
		Context->Release();
	}

	if(Device != nullptr)
	{
		Device->Release();
	}

	if(Swap != nullptr)
	{
		Swap->Release();
	}

	if(Target != nullptr)
	{
		Target->Release();
	}

	if(DepthStencilBuffer != nullptr)
	{
		DepthStencilBuffer->Release();
	}

	if(DepthStencilState != nullptr)
	{
		DepthStencilState->Release();
	}

	if(DepthStencilView != nullptr)
	{
		DepthStencilView->Release();
	}

	if(DepthStencilView != nullptr)
	{
		AlphaBlendState->Release();
	}
}
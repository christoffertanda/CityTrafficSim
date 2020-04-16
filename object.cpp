#include "object.h"

object::object(obj_file *Asset, vec3 Position, vec3 Scale, vec3 Rotation, vec4 Color) :
	Asset(Asset)
{
	// TODO(Cristoffer): Even if the data exists in memory it gets copier
	// per object at the moment. The because the data has to be padded in
	// such a way that the graphics driver can read it. Implement a better 
	// way down the line.

	SetInitialModel(Position, Scale, Rotation);

	struct cb
	{
		XMMATRIX MVP;
		XMMATRIX Model;
		XMFLOAT4 AmbientLight;
		XMFLOAT3 LightPosition;

	} VS_Input;

	// NOTE(Cristoffer): Store vertex data.
	for(auto Vert : Asset->Vertices)
	{
		vertex Vertex;

		Vertex.Position = vec3(Vert.x, Vert.y, Vert.z);
		Vertex.Normal = vec3(0.5f, 0.5f, 0.5f);
		Vertex.Color = Color;
		Vertex.HighlightColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);

		Vertices.push_back(Vertex);
	}

	// NOTE(Cristoffer): Store the normals.
	for(auto Vertex : Asset->Normals)
	{
		Normals.push_back(Vertex);
	}
	
	// NOTE(Cristoffer): Fill the normal vertices based on mapped index.
	for(auto Element : Asset->FaceIndices)
	{
		uint32 VertexIndex = Element.Position;
		uint32 NormalIndex = Element.Normal;

		Vertices.at(VertexIndex).Normal = Normals.at(NormalIndex);
	}

	// NOTE(Cristoffer): Vertex indicies for index buffer.
	for(auto Indicies : Asset->FaceIndices)
	{
		Indices.push_back(Indicies.Position);
	}

	Shader = new shader(L"vehicle_vs.cso", L"vehicle_ps.cso");
	Shader->AddInputElement("POSITION", DXGI_FORMAT_R32G32B32_FLOAT);
	Shader->AddInputElement("COLOR", DXGI_FORMAT_R32G32B32A32_FLOAT);
	Shader->AddInputElement("NORMAL", DXGI_FORMAT_R32G32B32_FLOAT);
	Shader->AddInputElement("HIGHLIGHTCOLOR", DXGI_FORMAT_R32G32B32A32_FLOAT);
	Shader->CommitInputElements();

	Texture = nullptr;

	VertexBuffer = new vertex_buffer(Vertices.data(), sizeof(vertex), (uint32)Vertices.size(), accessibility::Dynamic);
	VertexBuffer->AddIndexBuffer(Indices.data(), sizeof(uint32), (uint32)Indices.size());

	ConstantBuffer = new constant_buffer(&VS_Input, sizeof(cb));
}

void object::Draw()
{
	struct cb
	{
		XMMATRIX MVP;
		XMMATRIX Model;
		XMFLOAT4 AmbientLight;
		XMFLOAT3 LightPosition;

	} VS_Input;

	VS_Input.MVP = XMMatrixTranspose(Model * XMMATRIX(camera::GetViewMatrix()) * XMMATRIX(camera::GetProjectionMatrix()));
	VS_Input.Model = XMMatrixTranspose(Model);
	VS_Input.AmbientLight = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	//VS_Input.LightPosition = XMFLOAT3(Camera.GetPositionX(), Camera.GetPositionY(), Camera.GetPositionZ());
	VS_Input.LightPosition = XMFLOAT3(light_source::Position.x, 
									  light_source::Position.y, 
									  light_source::Position.z);

	//Texture->Bind();
	VertexBuffer->Bind();
	Shader->Bind();
	ConstantBuffer->Bind();
	ConstantBuffer->Update(&VS_Input);

	direct3d::GetContext()->DrawIndexed(VertexBuffer->GetIndexCount(), 0, 0);
}
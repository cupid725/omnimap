#ifndef __OMNIMAP_CHANNEL_D3D11_H__
#define __OMNIMAP_CHANNEL_D3D11_H__

#include "OmniMapBase.h"
#include <d3dx9math.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <d3dx11effect.h>
#include <dxerr.h>


#define OMNIMAP_V(x) { hr = x; if( FAILED(hr) ) { LogSystem()->ReportError("D3D11 Error:%s %d %d %s\n",  __FILE__, (DWORD)__LINE__, hr, L#x, true ); } }


class OmniMapD3D11;
class OmniMapShaderD3D11;


/**
 * \brief Defines a single render channel for rendering a portion of the final composited 
 * screen image.  This is the DirectX11 implementaion derived from the abstract class OmniMapChannelBase.
 *
 * The OmniMap API's final rendered image is a composite of 1 to 6 render channels.  Those
 * channels' attributes are defined by the OmniMap_Channel class.  The primary attributes of
 * a channel are the camera frustum, and viewing angle which are defined in the cam_info 
 * public member.  It is also possible to have each channel displayed in an inset (heads up display)
 * on the final rendered image.  
 * 
 * Scriptable parameters can be set and accessed through the OmniMapChannelBase::access
 * member variable.  These variable can also be set via the LUA configuration files using the LUA function:
 * SetChannelParameter. See the  OmniMapChannelBase class for base class defined scriptable parameters.
 * The DirectX specific channel attributes are listed here.  They are:
 * <ul>
 *	<li>
 *		<b>MultiSampleQuality</b> : The value set here will be used for the Direct3D call 
 *			CreateRenderTarget when the render target for the channels are set up. 
 *			Be careful not to set this number to high.
 *			Read the Microsoft documentation on CreateRenderTarget for more information.
 *	</li>
 * </ul>
 */
class OMNIMAP_API OmniMapChannelD3D11 : public OmniMapChannelBase
{

public: 

	friend class OmniMapD3D11;

	/**
	 * \brief Construct an OmniMapChannelD3D Object.
	 *
	 * @param _d3dDevice The ID3D11Device the application is using for Direct3D rendering.
	 */
	OmniMapChannelD3D11(ID3D11Device * _d3dDevice, ID3D11DeviceContext *_d3dDeviceContext);
	/**
	 * Destroy this channel
	 */
	virtual ~OmniMapChannelD3D11();

	/**
	* \brief Initialize the channel.  
	*
	* In the DirectX 11 implemenation, this method creates a render target and depth stencil
	* to be used for rendering the image off screen into a dynamic texture.
	*/
	void Initialize();

	/**
	 * \brief Set up the graphics state for rendering this channel.
	 *
	 * This method needs to be called before rendering to the channel begins.  It sets
	 * the viewport to the size of this channel, and the render target to the channels off screen
	 * render target view and depth stencil view.
	 */
	virtual void beginRenderToChannel();
	/**
	 * \brief Return the matrix and graphics state to its condition prior to the call to
	 * OmniMapChannelBase::beginRenderToChannel.
	 */
	virtual void endRenderToChannel();
	/**
	 * \brief Set the near and far clipping planes for this channel.
	 *
	 * Sets the near and far clipping planes for the channel, and recalculate the left, right, top, and bottom
	 * planes.
	 * @param fnear The near clipping plane.
	 * @param ffar The far clipping plane.
	 */
	void SetNearFar(float fnear, float ffar);

	/**
	 * \brief Does nothing in the DirectX 10 implementation. 
	 *
	 * In DX 10 application programmers are expected
	 * to use OmniMapChannelD3D11::GetProjectionMatrix to get the correct projection matrix for this channel,
	 * then use that projection matrix when rendering the application's scene.
	 * 
	 */
	virtual void ApplyProjectionMatrix();
	/**
	 * \brief Does nothing in the DirectX 10 implementation. 
	 *
	 * In DX 10 application programmers are expected
	 * to use OmniMapChannelD3D11::GetViewMatrix to get the correct viewing offset matrix for this channel,
	 * then use that matrix when rendering the application's scene.
	 */
	virtual void ApplyViewMatrix();

	/**
	 * \brief Get the view matrix that represent's the camera rotation and position offset for this channel.
	 *
	 * @param vm A pointer to the D3DXMATRIX that will be set to the camera rotation and position
	 * offset for this channel.
	 */
	 virtual void  GetViewMatrix(D3DXMATRIX *vm);

	 /**
	 * \brief Get the left hand projection matrix that represent's the camera viewing frustum for this channel.
	 *
	 * @param vm A pointer to the D3DXMATRIX that will be set to the camera viewing frustum for this channel.
	 */
	 virtual void  GetProjectionMatrix(D3DXMATRIX *vm);

	 /**
	 * \brief Get the right hand projection matrix that represent's the camera viewing frustum for this channel.
	 *
	 * @param projMat A pointer to the D3DXMATRIX that will be set to the camera viewing frustum for this channel.
	 */
	 virtual void GetProjectionMatrixRH(D3DXMATRIX *projMat);

	/**
	 * \brief Get the projective texturing matrix used to project this channel onto the output screen shape.
	 *
	 * This method returns the matrix in a 16 element float array.
	 * @param m The returned matrix.
	 */
	virtual void GetProjTextMatrix(float m[16]);

	/**
	 * \brief Get the projection matrix.
	 *
	 * This method returns the projection matrix in a 16 element float array.
	 *
	 * @param m The returned matrix.
	 */
	virtual void GetProjectionMatrix(float m[16]);

	/**
	 * \brief Get the projective texturing matrix used to project this channel onto the output screen shape.
	 *
	 * This method returns the matrix in a D3DXMATRIX.
	 * @param ptMat The returned matrix.
	 */
	virtual void GetProjTextMatrix(D3DXMATRIX *ptMat);
	/**
	 * \brief Make the texture unit for this channel the active texture unit.
	 *
	 * Makes the texture unit for this channel the active texture unit(glActiveTextureARB)
	 * and then binds the texture id that contains the content for this channel to that
	 * active texture unit.
	 */
	virtual void BindTexture(int index);
	/**
	 * \brief Unbind the texture for this channel.
	 */
	virtual void UnbindTexture(int index);
	/**
	 * \brief Draw the contents of this channel to the heads up display.
	 */
	virtual void DrawHeadsUpDisplay();

  virtual void SetD3D11DeviceContext(ID3D11DeviceContext *_d3dDeviceContext);
	




protected:
	ID3D11Buffer *hudVertexBuffer;
	ID3D11Buffer *hudWireframeBuffer;
	D3DXMATRIX  saveViewMatrix;
	D3DXMATRIX  saveProjMatrix;
	D3D11_VIEWPORT vpSave;
	

protected:
public:
	/**
	 * \brief Set the shader to be used by this channel for rendering the
	 * heads-up display.
	 */
	virtual void setOMShader(OmniMapShaderD3D11 *shader);
	virtual void GetViewMatrix2(D3DXMATRIX *vm);
	virtual void SetUpPropertyAccess();
	DXGI_FORMAT getDSFormat();
	void CreateHUDSurface();
	
	float multiSampleQuality;
	int multiSampleCount;
	OmniMapShaderD3D11 *omShader;
	ID3D11Device       *d3dDevice;
  ID3D11DeviceContext *d3dDeviceContext;
	ID3D11Texture2D*        renderTargetTexture;
	ID3D11RenderTargetView* renderTargetView;
	ID3D11ShaderResourceView* renderTargetSRV;
	ID3D11Texture2D* channelShdrTexture;

	ID3D11Texture2D*        depthStencilTarget;
	ID3D11DepthStencilView* depthStencilView;

	ID3D11RenderTargetView* apOldRTVs;
  ID3D11DepthStencilView* pOldDS;

	virtual void PushSetupMatricies();
	virtual void PopSetupMatricies();

	
};

#endif
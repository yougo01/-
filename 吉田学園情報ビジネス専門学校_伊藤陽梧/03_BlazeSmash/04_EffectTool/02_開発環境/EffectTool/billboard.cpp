//===============================================
//
// ビルボード処理 (billboard.cpp)
// Author : 伊藤陽梧
//
//===============================================

//========================
// インクルードファイル
//========================
#include "billboard.h"
#include "manager.h"
#include "renderer.h"
#include "texture.h"

//========================================
// 静的メンバ変数宣言
//========================================
bool CBillboard::m_bAdditiveSynthesis = false;
bool CBillboard::m_bUseZBuffer = true;

//=============================================================================
// オーバーライドされたコンストラクタ
//=============================================================================
CBillboard::CBillboard(CScene::OBJTYPE objtype) :CScene(objtype)
{
	m_pTexture = NULL;
	m_pVtxBuff = NULL;
	D3DXMatrixIdentity(&m_mtxWorld);
	m_pos = DEFAULT_VECTOR;
	m_fsize = DEFAULT_VECTOR;
	m_rot = DEFAULT_VECTOR;
	m_scale = DEFAULT_SCALE;
	m_col = DEFAULT_COLOR;
	m_fTexX1 = 0.0f;
	m_fTexX2 = 1.0f;
	m_fTexY1 = 0.0f;
	m_fTexY2 = 1.0f;
	m_nAlphaTestBorder = DEFAULT_ALPHA_TEST_BORDER;
	m_fRotAngle = 0.0f;
}

//=============================================================================
// デフォルトコンストラクタ
//=============================================================================
CBillboard::CBillboard()
{
	m_pTexture = NULL;
	m_pVtxBuff = NULL;
	D3DXMatrixIdentity(&m_mtxWorld);
	m_pos = DEFAULT_VECTOR;
	m_fsize = DEFAULT_VECTOR;
	m_rot = DEFAULT_VECTOR;
	m_scale = DEFAULT_SCALE;
	m_col = DEFAULT_COLOR;
	m_fTexX1 = 0.0f;
	m_fTexX2 = 1.0f;
	m_fTexY1 = 0.0f;
	m_fTexY2 = 1.0f;
	m_nAlphaTestBorder = DEFAULT_ALPHA_TEST_BORDER;
	m_fRotAngle = 0.0f;
}

//=============================================================================
// デストラクタ
//=============================================================================
CBillboard::~CBillboard()
{
}

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CBillboard::Init(void)
{
	// デバイスを取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// 頂点バッファを生成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_3D)*NUM_VERTEX, D3DUSAGE_WRITEONLY, FVF_VERTEX_3D, D3DPOOL_MANAGED, &m_pVtxBuff, NULL);
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

	// 頂点情報の更新
	Update();

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void CBillboard::Uninit(void)
{
	// 頂点バッファを開放
	if (m_pVtxBuff != NULL)
	{
		m_pVtxBuff->Release();
		m_pVtxBuff = NULL;
	}

	// シーンの開放
	Release();
}

//=============================================================================
// 更新処理
//=============================================================================
void CBillboard::Update(void)
{
	// 変数宣言
	// 各頂点
	D3DXVECTOR3 vertex1;
	D3DXVECTOR3 vertex2;
	D3DXVECTOR3 vertex3;
	D3DXVECTOR3 vertex4;

	//==========================================================================================================
	//画像を、画像の中心を軸に回転させる
	//左上の頂点
	vertex1.x = -(m_fsize.x / 2)*cosf(m_fRotAngle)
		- ((m_fsize.y / 2))*sinf(m_fRotAngle);
	vertex1.y = -(m_fsize.x / 2)*sinf(m_fRotAngle)
		+ ((m_fsize.y / 2))*cosf(m_fRotAngle);
	vertex1.z = 0.0f;

	//右上の頂点
	vertex2.x = (m_fsize.x / 2)*cosf(m_fRotAngle)
		- ((m_fsize.y / 2))*sinf(m_fRotAngle);
	vertex2.y = (m_fsize.x / 2)*sinf(m_fRotAngle)
		+ ((m_fsize.y / 2))*cosf(m_fRotAngle);
	vertex2.z = 0.0f;

	//左下の頂点
	vertex3.x = -(m_fsize.x / 2)*cosf(m_fRotAngle)
		- (-(m_fsize.y / 2))*sinf(m_fRotAngle);
	vertex3.y = -(m_fsize.x / 2)*sinf(m_fRotAngle)
		+ (-(m_fsize.y / 2))*cosf(m_fRotAngle);
	vertex3.z = 0.0f;

	//右下の頂点
	vertex4.x = (m_fsize.x / 2)*cosf(m_fRotAngle)
		- (-(m_fsize.y / 2))*sinf(m_fRotAngle);
	vertex4.y = (m_fsize.x / 2)*sinf(m_fRotAngle)
		+ (-(m_fsize.y / 2))*cosf(m_fRotAngle);
	vertex4.z = 0.0f;
	//==========================================================================================================

	VERTEX_3D *pVtx = NULL;	// 頂点情報へのポインタ

							// 頂点データの範囲をロックし、頂点バッファへのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);	// この書式は変えないこと

												// 頂点座標の設定
	pVtx[0].pos = vertex1;
	pVtx[1].pos = vertex2;
	pVtx[2].pos = vertex3;
	pVtx[3].pos = vertex4;

	// 各頂点のテクスチャ座標を更新
	pVtx[0].tex = D3DXVECTOR2(m_fTexX1, m_fTexY1);
	pVtx[1].tex = D3DXVECTOR2(m_fTexX2, m_fTexY1);
	pVtx[2].tex = D3DXVECTOR2(m_fTexX1, m_fTexY2);
	pVtx[3].tex = D3DXVECTOR2(m_fTexX2, m_fTexY2);

	// 各頂点の色と法線ベクトルを更新
	for (int nCount = 0; nCount < NUM_VERTEX; nCount++)
	{
		pVtx[nCount].col = m_col;
		pVtx[nCount].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
	}

	//頂点データをアンロックする
	m_pVtxBuff->Unlock();
}

//=============================================================================
// 描画処理
//=============================================================================
void CBillboard::Draw(void)
{
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();
	D3DXMATRIX mtxRot, mtxTrans, mtxScale;

	// ライトを無効に
	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	// Zバッファを使用するフラグがfalseなら
	if (m_bUseZBuffer == false)
	{
		// Zバッファを無効化
		pDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
	}

	// 加算合成
	if (m_bAdditiveSynthesis == true)
	{
		// レンダーステート(加算合成にする)
		pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
		pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
	}

	//アルファテストを有効化
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);

	//アルファテスト基準値の設定
	pDevice->SetRenderState(D3DRS_ALPHAREF, m_nAlphaTestBorder);

	//アルファテストの比較方法の設定
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);

	// ワールドマトリクスの初期化
	D3DXMatrixIdentity(&m_mtxWorld);

	// 回転を決めてないなら、カメラに対して常に正面
	if (m_rot == DEFAULT_VECTOR)
	{
		D3DXMatrixScaling(&mtxScale, m_scale.x, m_scale.y, m_scale.z);
		D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxScale);
		pDevice->GetTransform(D3DTS_VIEW, &mtxRot);
		D3DXMatrixInverse(&mtxRot, NULL, &mtxRot);

		mtxRot._41 = 0.0f;
		mtxRot._42 = 0.0f;
		mtxRot._43 = 0.0f;

		D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxRot);

		D3DXMatrixTranslation(&mtxTrans, m_pos.x, m_pos.y, m_pos.z);
		D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxTrans);
	}
	else
	{
		// 向きを反映
		D3DXMatrixRotationYawPitchRoll(&mtxRot, m_rot.y, m_rot.x, m_rot.z);
		D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxRot);

		// 位置を反映
		D3DXMatrixTranslation(&mtxTrans, m_pos.x, m_pos.y, m_pos.z);
		D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxTrans);
	}

	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

	pDevice->SetStreamSource(0, m_pVtxBuff, 0, sizeof(VERTEX_3D));

	pDevice->SetFVF(FVF_VERTEX_3D);

	pDevice->SetTexture(0, m_pTexture);

	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, NUM_POLYGON);

	// テクスチャをNULLに
	pDevice->SetTexture(0, NULL);

	//アルファテストを無効化
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	// 加算合成を戻す
	if (m_bAdditiveSynthesis == true)
	{
		// レンダーステート(加算合成を戻す)
		pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
		pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

		// 加算合成のフラグをfalseに戻す
		m_bAdditiveSynthesis = false;
	}

	// Zバッファを使用するフラグがfalseなら
	if (m_bUseZBuffer == false)
	{
		// Zバッファを有効化
		pDevice->SetRenderState(D3DRS_ZENABLE, TRUE);

		// Zバッファを無効化するフラグをtrueに戻す
		m_bUseZBuffer = true;
	}

	// ライトを有効化
	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
}

//=============================================================================
// テクスチャをバインドする処理
//=============================================================================
void CBillboard::BindTexture(const int nNumTexture)
{
	CTexture *pTexture = CManager::GetTexture();
	m_pTexture = pTexture->GetInfo(nNumTexture)->pTexture;
}

//=============================================================================
// 頂点カラーを直接設定する処理
//=============================================================================
void CBillboard::SetColorVertex(D3DXCOLOR col)
{
	// メンバ変数に結びつける
	m_col = col;

	// 頂点情報へのポインタ
	VERTEX_3D *pVtx = NULL;

	// 頂点データの範囲をロックし、頂点バッファへのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);    // この書式は変えないこと

												 // 各頂点の色を更新
	for (int nCount = 0; nCount < NUM_VERTEX; nCount++)
	{
		pVtx[nCount].col = m_col;
	}

	//頂点データをアンロックする
	m_pVtxBuff->Unlock();
}

//=============================================================================
// テクスチャ座標を更新する処理
//=============================================================================
bool CBillboard::SetUpdateTexLeftToRight(int nMaxPattern)
{
	// 一周したかどうか
	bool bOneRound = false;

	// テクスチャ座標を更新する割合
	float fUpdateRate = 0.0f;

	// パターンによって、更新量を変える
	fUpdateRate = 1.0f / (float)nMaxPattern;

	// それぞれのテクスチャ座標Xに加える
	m_fTexX1 += fUpdateRate;
	m_fTexX2 += fUpdateRate;

	// テクスチャ座標の右側が1.0fを超えたら
	if (m_fTexX2 >= 1.0f)
	{
		// 一周のフラグをtrueに
		bOneRound = true;
	}

	return bOneRound;
}

const VertexPositionNormalTangentColorTexture g_sphereVB[] = {
    { XMFLOAT3(0,0,0), XMFLOAT3(0,1,0), XMFLOAT4(1,0,0,1),
      0xFFFFFFFF, XMFLOAT2(0,0) },
    { XMFLOAT3(4,0,0), XMFLOAT3(0,1,0), XMFLOAT4(1,0,0,1),
      0xFFFFFFFF, XMFLOAT2(1,0) },
    { XMFLOAT3(0,0,4), XMFLOAT3(0,1,0), XMFLOAT4(1,0,0,1),
      0xFFFFFFFF, XMFLOAT2(0,1) }
};
const uint16_t g_sphereIB[] = {
    0,1,2
};
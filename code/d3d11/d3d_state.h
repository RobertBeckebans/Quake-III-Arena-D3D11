#pragma once

// @pjb: stores the D3D state. The game is set up like a state machine so we'll be doing the same.
struct d3dState_t {
    // Init-time state
    DXGI_SWAP_CHAIN_DESC swapChainDesc;
    D3D_FEATURE_LEVEL featureLevel;

    // Run-time state
    float modelViewMatrix[16];
    float projectionMatrix[16];
};

extern d3dState_t d3dState;

#pragma once

#include <agile.h>

namespace Win8
{
    void InitTimer();
    int GetTime();
    size_t CopyString( Platform::String^ str, char* dst, size_t dstLen );
    Platform::String^ CopyString( const char* src );
    size_t MultiByteToWide( const char* src, wchar_t* dst, size_t dstLen );
    void SetCommandLine( Platform::Array<Platform::String^>^ args );
    void Throw( HRESULT hr, Platform::String^ str );
    Windows::Foundation::IAsyncOperation<Windows::UI::Popups::IUICommand^>^ DisplayException( Platform::Exception^ ex );
    void SetCommandLine( Platform::Array<Platform::String^>^ args );
    void PostQuitMessage();
   
    template<class Type> IUnknown* GetPointer( Type^ obj )
    {
        Platform::Agile<Type> agile( obj );
        IUnknown* ptr = reinterpret_cast<IUnknown*>( agile.Get() );
        ptr->AddRef();
        return ptr;
    }

    template<class Type> Type^ GetType( IUnknown* obj )
    {
        Platform::Details::AgileHelper<Type> agile( reinterpret_cast<__abi_IUnknown*>( obj ), true );
        return agile;
    }

}

<?php

namespace App\Http\Requests;

use Illuminate\Foundation\Http\FormRequest;

class HardwareRequest extends FormRequest
{
    /**
     * Determine if the user is authorized to make this request.
     */
    public function authorize(): bool
    {
        return true;
    }

    /**
     * Get the validation rules that apply to the request.
     *
     * @return array<string, \Illuminate\Contracts\Validation\ValidationRule|array|string>
     */
    public function rules(): array
    {
        return [
            'temperature' => 'nullable|numeric|required_with:humidity',
            'humidity' => 'nullable|numeric|required_with:temperature|min:1',
            'uid' => 'nullable|string|required_with:action',
            'action' => 'nullable|string|required_with:uid|in:ADD,REMOVE',
        ];
    }
}

<?php

namespace App\Rules;

use Illuminate\Contracts\Validation\Rule;

class NotInPastDatetime implements Rule
{
    public function passes($attribute, $value)
    {
        return strtotime($value) > time();
    }

    public function message()
    {
        return 'The :attribute must not be in the past.';
    }
}

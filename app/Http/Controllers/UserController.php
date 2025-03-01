<?php

namespace App\Http\Controllers;

use App\Http\Requests\UserRequest;
use App\Models\User;
use Illuminate\Http\Request;
use Illuminate\Support\Facades\Auth;

class UserController extends Controller
{
    /**
     * Update the specified resource in storage.
     */
    public function update(UserRequest $request)
    {
        if ($request->password == $request->confirmPassword) {
            $user = User::find(Auth::id());
            $user->name = $request->name;
            $user->email = $request->email;
            $user->password = bcrypt($request->password);
            $user->update();
            return redirect()->back();
        } else {
            return redirect()->back()->withErrors('Passwords not match');
        }
    }
}

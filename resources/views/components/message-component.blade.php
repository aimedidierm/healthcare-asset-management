@if (session('success'))
<div class="mb-4 p-4 bg-green-200 text-green-800 rounded">
    {{ session('success') }}
</div>
@elseif ($errors->any())
<div class="mb-4 p-4 bg-red-200 text-red-800 rounded">
    <ul>
        @foreach ($errors->all() as $error)
        <li>{{ $error }}</li>
        @endforeach
    </ul>
</div>
@endif